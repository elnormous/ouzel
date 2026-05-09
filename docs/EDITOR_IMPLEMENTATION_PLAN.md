# Fluxion Scene Editor — Implementációs Terv

Cél: a `nervous-hamilton-a271ee` worktree-ben a Fluxion motort scene-editor-ready
állapotba hozni úgy, hogy **minden eddig támogatott platform** (Windows, macOS,
Linux, iOS, tvOS, Android, Emscripten) és **minden Fluxion renderer**
(Direct3D 11, OpenGL 2/3/4, OpenGL ES 2/3, Metal) tovább működjön, és minden
új feature a meglévő alrendszereket használja, ne duplikálja.

A tervet kilenc fázisra bontottuk. A fázisok **sorrendben** épülnek
egymásra; bármelyiket átugrani későbbi visszafordulást igényel.

---

## 0. Architektúra-szintű döntések (mindegyik fázist érinti)

### 0.1 ImGui platform-backendek elkerülése — egységes input-routing

A `external/imgui/backends/` tartalmazza a `_win32` és `_android` platform
backendeket, de Linux/X11, macOS, iOS, Emscripten nincs lefedve. A
heterogenitás elkerülése + a Fluxion input-rendszer már létező egységessége
miatt a döntés:

> **A platform-backendek közül EGYIKET SEM használjuk.** Helyette egyetlen
> `imgui::FluxionPlatformBackend` osztály feed-eli az `ImGuiIO`-t a Fluxion
> `EventDispatcher` eseményeiből. Egyetlen kód, mind a 7 platformon működik.

Renderer-backendekkel viszont ezt nem csináljuk: a stock ImGui
renderer-backendek (vertex buffer, blend state, texture upload) optimalizáltak
és karbantartottak, nincs értelme újraírni. **A renderer-backendeket
módosítatlan stock formában használjuk**, csak a Fluxion `RenderDevice`
absztrakcióból kinyerjük a natív handle-eket.

### 0.2 Threading szabály

A Fluxion-ban külön update-thread fut (`Engine::updateThread`). Az ImGui nem
thread-safe; minden ImGui hívás egyetlen szálon történjen. Választás:

> **Az ImGui mindig a render-szálon fut.** Az input-eseményeket egy
> lock-free queue-ba (`engine/thread/Spsc.hpp` – ha nincs, fázis 1.4 hozza
> létre) tesszük az input-szálon, és a render-szál `ImGui::NewFrame()` előtt
> drainezi. A user-kód (`onImGui()`) a render-szálon hívódik.

### 0.3 Editor-mód vs runtime-mód

Az editor csak akkor használja az új API-kat, ha a `FLUXION_EDITOR_BUILD`
defined. Runtime-buildbe semmi editor-only kód nem kerül (sem ImGui, sem
reflection metaadat, sem szerializáció editor-oldali része). Ez biztosítja
hogy a meglévő samplek méretben/teljesítményben **regresszió-mentesek**.

A reflection alapját (komponens type-name, factory) viszont **runtime is
használja** a deszerializáláshoz, ezért az nem editor-only.

### 0.4 Build-rendszer egységesítés

Minden új source-fájl bekerül **mindhárom** build-rendszerbe:
- `engine/libfluxion.vcxproj` (+ `.filters`) — Visual Studio
- `engine/libouzel.xcodeproj/project.pbxproj` — Xcode (macOS/iOS/tvOS)
- `engine/Makefile` — GNU make (Linux/macOS CLI/Emscripten)
- `engine/jni/Android.mk` — Android NDK ndkBuild

Minden fázis végén `git grep` a fájlnévre mind a négy helyen.

**Xcode pbxproj megjegyzés**: a `.pbxproj` fájl belső UUID-azonosítókkal hivatkozik
fájlokra, és a kézi szerkesztés könnyen korrupt projektet eredményez. A Xcode-os
verziófrissítések során a hozzáadás Xcode UI-on keresztül történjen
(File → Add Files to "libouzel"…), vagy a [`xcodeproj` Ruby gem](https://github.com/CocoaPods/Xcodeproj)
parancssoros API-jával. A Makefile build viszont működik macOS-en is (CLI buildhez),
így a Xcode-frissítés kihagyható ha nincs Xcode-szintű igény.

---

## 1. fázis — ImGui integráció (≈ 1.5 hét)

**Output:** A `samples.exe -sample sprites` futtatás közben felugrik egy
ImGui demo ablak, mind a 4 rendereren (D3D11, OGL3, OGL ES 3, Metal),
mind a 7 platformon. Az ablak interaktív: ablakot lehet húzni, slider-eket
mozgatni, és nem zavarja a sprite rendereléi alatta.

### 1.1 Új modul: `engine/gui/imgui/`

```
engine/gui/imgui/
├── ImGuiContext.hpp / .cpp        — Fluxion-szintű kontextus wrapper
├── ImGuiRenderBackend.hpp         — absztrakt interface
├── ImGuiRenderBackendD3D11.cpp    — wraps imgui_impl_dx11
├── ImGuiRenderBackendOGL2.cpp     — wraps imgui_impl_opengl2
├── ImGuiRenderBackendOGL3.cpp     — wraps imgui_impl_opengl3 (OGL3/4 + ES2/3)
├── ImGuiRenderBackendMetal.mm     — wraps imgui_impl_metal
├── ImGuiRenderBackendNull.cpp     — empty/headless
├── ImGuiPlatformBackend.hpp / .cpp — egységes input-feed (minden platform)
└── ImGuiKeyMap.hpp                 — Fluxion::input::Keyboard::Key → ImGuiKey
```

### 1.2 `ImGuiContext` API

```cpp
namespace fluxion::gui::imgui {
class Context {
public:
    Context(graphics::Graphics&, input::InputManager&, EventDispatcher&);
    ~Context();

    // user-callback, a render-szálon hívódik minden frame
    using DrawCallback = std::function<void()>;
    void setDrawCallback(DrawCallback);

    // editor → runtime átkapcsolás (input passthrough on/off)
    void setCaptureInput(bool);
    bool wantsKeyboard() const;  // io.WantCaptureKeyboard
    bool wantsMouse() const;     // io.WantCaptureMouse

    // DPI / font reload
    void setDpiScale(float);
    void rebuildFontAtlas();
};
}
```

A `Context` belül EventHandler-eket regisztrál a `EventDispatcher`-en, és
push-olja az eseményeket a render-szál SPSC queue-jába.

### 1.3 RenderDevice handle-expozíció (renderer-enkénti módosítás)

A Fluxion-`RenderDevice` jelenleg nem ad ki natív handle-t. Adunk egy
opcionális, csak editor-buildben elérhető virtuális metódust:

```cpp
// engine/graphics/RenderDevice.hpp
class RenderDevice {
    virtual void* getNativeDevice() const { return nullptr; }
    virtual void* getNativeContext() const { return nullptr; }
    virtual void* getNativeQueue() const { return nullptr; }     // Metal-only
};
```

Override-olja:
- `D3D11RenderDevice` → `ID3D11Device*`, `ID3D11DeviceContext*`
- `OGLRenderDevice` → nullptr (OGL backendnek context-binding elég)
- `MetalRenderDevice` → `id<MTLDevice>`, `id<MTLCommandQueue>`

Csak a backend cast-ja, nem szivárog ki az API-n túlra (a publik header
`void*`-ot ad vissza).

### 1.4 Render-pass beillesztése

A Fluxion render-loop végén, az utolsó scene-pass után, **új render-pass**
az ImGui-nak. Ez nem RenderTarget-be megy, hanem közvetlenül a backbufferre,
viewport-méret = window-méret, depth nélkül.

Módosítandó: [engine/graphics/Graphics.cpp](../engine/graphics/Graphics.cpp)
és a per-renderer device:
- `D3D11RenderDevice::present()` előtt: `ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData())`
- `OGLRenderDevice::present()` előtt: `ImGui_ImplOpenGL3_RenderDrawData(...)`
- `MetalRenderDevice` command buffer-be: `ImGui_ImplMetal_RenderDrawData(...)`

### 1.5 Per-platform window-handle expozíció

ImGui Win32 backend-et **nem** használjuk, de néhány platformon az ablakot
ImGui-nak ismernie kell (pl. multi-viewport-hoz; most disabled hagyjuk).
A Fluxion `Window::getNativeHandle()` már megvan – nem kell változtatni.

### 1.6 Sample-bővítés

Új `ImGuiSample.cpp` a samples-ben:
```cpp
class ImGuiSample : public Scene {
    gui::imgui::Context imguiCtx;
    ImGuiSample() : imguiCtx{*engine->getGraphics(), ...} {
        imguiCtx.setDrawCallback([]{ ImGui::ShowDemoWindow(); });
    }
};
```
Ezt a `samples/main.cpp`-be fel kell venni `imgui` rövid név alatt.

### 1.7 Verification (mindegyik platformon külön)

| Platform | Renderer | Hogyan |
|---|---|---|
| Windows | D3D11 | `samples.exe -sample imgui` → demó ablak |
| Windows | OGL3 | konfig: OGL force, ugyanaz |
| macOS | Metal | Xcode samples target, run |
| macOS | OGL3 | konfig: OGL force |
| Linux | OGL3 | `make` + `./samples -sample imgui` |
| iOS | Metal | Xcode iOS target |
| Android | OGL ES 3 | gradle assembleDebug + run |
| Emscripten | OGL ES 3 / WebGL | `make PLATFORM=emscripten` + browser |

Minden esetben: demó ablak megjelenik, slider-mozgatás működik, sprite
mögötte rendereli magát, és a sample 30+ másodpercig fut crash nélkül.

---

## 2. fázis — Reflection & komponens metaadat (≈ 1.5 hét)

**Output:** Bármely `Component`-en futtatható `typeName()`, `inspect()`,
`serialize()`. Új komponens hozzáadása = 1 macro a `.cpp` fájlban.

### 2.1 Új modul: `engine/reflection/`

```
engine/reflection/
├── TypeRegistry.hpp / .cpp   — globális type-name → factory map
├── Reflectable.hpp           — base mixin
├── PropertyInfo.hpp          — name, type, getter/setter
├── Macros.hpp                — FLUXION_COMPONENT, FLUXION_PROPERTY
└── Inspector.hpp             — abstract interface (UI-független)
```

### 2.2 Macro-API

```cpp
// SpriteRenderer.cpp
FLUXION_COMPONENT(SpriteRenderer, "scene.SpriteRenderer")
FLUXION_PROPERTY(SpriteRenderer, color, "Color")
FLUXION_PROPERTY(SpriteRenderer, opacity, "Opacity", 0.0f, 1.0f)  // min/max
FLUXION_PROPERTY_ASSET(SpriteRenderer, texture, "Texture", AssetType::Image)
```

A macro generál:
- `static const char* typeName_static() { return "scene.SpriteRenderer"; }`
- `const char* typeName() const override { return typeName_static(); }`
- automatikus regisztráció a `TypeRegistry`-be statikus init-ként
- property-listát (név, getter, setter, típus-tag, opcionális min/max)
- `serialize(Serializer&)` és `inspect(Inspector&)` automatikusan

### 2.3 Inspector interface

A property-list bejárása UI-független, hogy az editor (ImGui) és a
deszerializáló is tudja használni:

```cpp
class Inspector {
public:
    virtual void floatField(const char* name, float* val, float min, float max) = 0;
    virtual void colorField(const char* name, math::Color* val) = 0;
    virtual void vec3Field(const char* name, math::Vector3F* val) = 0;
    virtual void assetField(const char* name, AssetReference* ref, AssetType) = 0;
    virtual void boolField(const char* name, bool* val) = 0;
    virtual void enumField(const char* name, int* val, const char* const* options, size_t n) = 0;
    // ...
};
```

Implementáció:
- `ImGuiInspector` — ImGui widget-ekhez
- `JsonInspector` (read/write) — szerializáláshoz (lásd 3. fázis)

Ez a kulcsdöntés: **egy property-deklaráció — két felhasználás**. Új mező
hozzáadásakor automatikusan editor-szerkeszthető ÉS perzisztens.

### 2.4 Komponens-osztályok átállítása (~12 osztály)

A `engine/scene/`-ben minden komponens kap macrót:
`SpriteRenderer`, `ShapeRenderer`, `TextRenderer`, `StaticMeshRenderer`,
`SkinnedMeshRenderer`, `ParticleSystem`, `Camera`, `Light`, `Animator`.
Plusz a GUI widget-ek a `engine/gui/`-ban.

### 2.5 Kompatibilitás

A meglévő samplek `addComponent(&sprite)` formában használják — ez nem
változik. A reflection csak addícionális. A statikus regisztráció `inline
namespace`-ben, hogy ne legyen ODR-ütközés.

### 2.6 Verification

- `TypeRegistry::getInstance().listTypes()` → összes komponens listája
- `TypeRegistry::create("scene.SpriteRenderer")` → pointer egy üres
  SpriteRenderer-re
- Egyetlen unit test: kerekítve serialize→deserialize→serialize, két JSON
  byte-azonos.

---

## 3. fázis — UUID-k és scene szerializáció (≈ 1 hét)

**Output:** `Scene::saveToFile(path)` és `loadFromFile(path)` egy
JSON-formátumba ment és visszatölt. Minden Actor-nak stabil 128-bites ID-je.

### 3.1 Actor ID

```cpp
// engine/scene/Actor.hpp
class Actor {
    Uuid id_;  // 128-bit (engine/utils/Uuid.hpp – új)
public:
    Uuid getId() const { return id_; }
    void setId(Uuid id) { id_ = id; }  // csak deszerializáláshoz
};
```

`Uuid` v4 generátor: platform-specifikus
- Windows: `CoCreateGuid`
- macOS/iOS: `CFUUIDCreate`
- Linux: `/proc/sys/kernel/random/uuid` vagy `uuid_generate` (libuuid)
- Android: `java.util.UUID` JNI hívás
- Emscripten: `crypto.randomUUID()` Emscripten EM_JS-en át

Mind már létező Fluxion-platform-API-kon keresztül.

### 3.2 Scene-bejárás és lookup

```cpp
class Scene {
    std::unordered_map<Uuid, Actor*> actorIndex_;
public:
    Actor* findActor(const Uuid&) const;
    void registerActor(Actor*);    // Layer::addChild belül hívja
    void unregisterActor(Actor*);
};
```

### 3.3 JSON-formátum

```json
{
  "fluxion_scene_version": 1,
  "actors": [
    {
      "id": "550e8400-...",
      "name": "Player",
      "parent": null,
      "transform": { "position": [0,0,0], "rotation": [...], "scale": [1,1,1] },
      "components": [
        {
          "type": "scene.SpriteRenderer",
          "properties": {
            "color": [1,1,1,1],
            "opacity": 1.0,
            "texture": { "type": "asset_ref", "guid": "..." }
          }
        }
      ]
    }
  ]
}
```

A `JsonInspector` osztály implementálja az `Inspector`-t úgy, hogy minden
`floatField` hívás kiír / beolvas egy JSON kulcsot. Egyetlen API két
irányban (write / read mód).

### 3.4 Asset-referenciák

Az Actor-ok közvetlen pointert tartanak Texture/Sound objektumokra. Ez
nem perzisztálható. Bevezetünk:

```cpp
// engine/assets/AssetReference.hpp
struct AssetReference {
    Uuid assetGuid;
    std::shared_ptr<void> resolved;  // cache-elt feloldás
};
```

Az `assets::Bundle`-t bővítjük: minden betöltött asset kap GUID-ot
(`assets.json` vagy `.meta` melléfájl). Új `meta`-fájl konvenció:
`fire.png` mellé `fire.png.meta` egyetlen GUID-dal. A `.meta` fájlokat
verziókövetjük.

### 3.5 Build-step: GUID generálás

A `tools/fluxion.exe` (már build-elt eszköz!) kap új parancsot:
`fluxion meta-generate Resources/` → minden GUID nélküli asset mellé
készít `.meta`-t.

### 3.6 Verification

- Sprites sample scene-jét mentsük → töltsük vissza → re-rendereljük.
  Pixelegyezés (lossy: nem kell pixel-perfect, de a sprite ott van ahol volt).
- `git diff` az exportált JSON-on stabil legyen (deterministikus key-sorrend).

---

## 4. fázis — Frame stepping és Edit/Play mode (≈ 1 hét)

**Output:** Az engine egy host-thread-ből vezérelhető (egy frame egyszerre),
a játékidő külön a render-időtől. Edit-módban a játéklogika nem fut,
csak a render. Play-módban minden mint korábban.

### 4.1 Engine-thread refaktor

[engine/core/Engine.cpp:609](../engine/core/Engine.cpp) `updateThread`-et
ki kell egészíteni egy `external-tick` üzemmóddal:

```cpp
// engine/core/Engine.hpp
class Engine {
    enum class TickMode { Internal, External };
    void setTickMode(TickMode);
    void tick(float dt);   // csak ha External — egy logikai frame
    void render();         // mindig hívható, függetlenül a tick-től
};
```

Internal mód = jelenlegi viselkedés (a worker-thread maga lép). External
mód = a host-alkalmazás (=editor) hívja a `tick()`-et frame-enként.

### 4.2 Edit/Play state-machine

```cpp
class EditorMode {
    enum class State { Editing, Playing, Paused };
    State currentState;

    // Play indításakor: scene snapshot a 3. fázis szerializálással
    // Stop: snapshot visszatöltése
    void enterPlay();
    void exitPlay();
    void pause();
    void resume();
    void step();  // egy frame manuálisan
};
```

Kulcs: a Play mode belépésekor szerializáljuk a scene-t in-memory JSON-ra,
és Stop-kor visszatöltjük. Így nem kell "deep copy" az egész scene
graph-ról — a 3. fázis szerializálási kódot újrahasznosítjuk.

### 4.3 Időskála

```cpp
class Engine {
    void setTimeScale(float);  // 0.0 = pause, 1.0 = normal, 2.0 = 2x
};
```

A `update(dt)` belül `dt *= timeScale_`. A render mindig real-time-ban fut.

### 4.4 Verification

- Editor: pause → kamera pörgetés → render frissül, sprite-ok állnak.
- Play → space-bar → step → egy lépés.
- Time-scale 0.25x → minden 4× lassan.

---

## 5. fázis — Picking, selection, gizmók (≈ 1.5 hét)

**Output:** Az editor viewport-ban kattintással kijelölhető Actor, és
move/rotate/scale gizmók megjelennek a kijelölés körül.

### 5.1 Math kibővítés

Új `engine/math/Ray.hpp`:
```cpp
template<class T> struct Ray {
    Vector<T, 3> origin, direction;
    bool intersects(const Box<T, 3>&, T& outT) const;
    bool intersects(const Plane<T>&, T& outT) const;
    bool intersectsTriangle(...) const;
};
```

Camera kibővítés:
```cpp
// Camera.hpp
math::RayF screenToRay(const Vector2F& screenPos) const;  // viewport-koord. → ray
```

A meglévő `convertNormalizedToWorld` lényegében ezt csinálja — de csak
egy pontot ad vissza, kell hogy egy ray legyen (közeli + távoli sík).

### 5.2 3D picking pipeline

```cpp
// engine/scene/Picker.hpp (új)
class Picker {
    PickResult pick(const Ray& ray, const Scene& scene) const;
};

struct PickResult {
    Actor* actor;
    Component* component;
    Vector3F worldPos;
    float distance;
};
```

Hierarchikus: először az Actor-AABB-vel teszteli, ha hit, akkor
mesh esetén triangle-pontossággal. Sprite/2D komponensek a meglévő
`Layer::pickActor` 2D logikáját használják, csak `Ray::intersectsPlane`-en
keresztül.

### 5.3 AABB-számítás komponensekre

Új virtuális metódus:
```cpp
class Component {
    virtual std::optional<Box3F> getBoundingBox() const { return std::nullopt; }
};
```

Override:
- `SpriteRenderer` → texture méretéből
- `StaticMeshRenderer` → mesh vertex-ből
- `ShapeRenderer` → akkumulált alakzatok bounding-box-a

### 5.4 Selection state

Az editor saját state-jében (NEM a scene-ben), egy `std::vector<Uuid>`
selection-set. Eseménykibocsátás (`SelectionChanged`) ImGui-panel
frissítéshez.

### 5.5 Gizmók

Új `engine/scene/Gizmo.hpp` osztály, három fajtával:
- `TranslateGizmo` (3 tengely)
- `RotateGizmo` (3 ring)
- `ScaleGizmo` (3 tengely + uniform middle)

Implementáció:
- A `ShapeRenderer`-t újrahasznosítjuk az immediate-mode rajzoláshoz.
- Külön `RenderPass`, depth-test off (mindig fent), screen-space scaling
  (a kamera-távolságtól függetlenül azonos pixelméret).
- Hit-test a tengelyekre `Ray vs Cylinder` egyszerű approximációval
  (tengely + sugár).
- Drag során a `Picker` által visszaadott `worldPos` és a tengely-irány
  alapján számított delta.

A gizmók **nem** a scene-be kerülnek (nem mentődnek). Külön
`EditorOverlayLayer` réteg.

### 5.6 Verification

- 3D Perspective sample-en kattintás a kockára → kockát kijelöli
- Move-gizmo X-tengelyét húzva → kocka X-en mozog
- Sprites sample-en sprite-kattintás → kijelölés (2D path)

---

## 6. fázis — Undo/Redo (≈ 3-5 nap)

**Output:** Minden editor-művelet visszavonható (Ctrl+Z) és
újra-előrehozható (Ctrl+Shift+Z), korlátlan mélységig (gyakorlatban
történet-limit, pl. 200 lépés).

### 6.1 Command interface

```cpp
// engine/editor/Command.hpp
class Command {
public:
    virtual ~Command() = default;
    virtual void apply() = 0;
    virtual void revert() = 0;
    virtual std::string label() const = 0;  // "Move Actor", "Add Component"
    virtual bool mergeWith(const Command&) { return false; }  // egymás utáni mozgások összevonása
};
```

### 6.2 History stack

```cpp
class CommandHistory {
    std::vector<std::unique_ptr<Command>> stack_;
    size_t cursor_;  // hányadik utáni állapotban vagyunk
    size_t maxSize_ = 200;
public:
    void execute(std::unique_ptr<Command>);  // apply + push
    void undo();
    void redo();
    void clear();  // pl. Play-belépésnél
};
```

### 6.3 Konkrét command-ok (kezdetnek elég ~6 fajta)

- `TransformActorCommand` — pozíció/rotáció/scale változás (mergeable)
- `AddActorCommand` / `RemoveActorCommand`
- `ReparentActorCommand`
- `AddComponentCommand` / `RemoveComponentCommand`
- `SetPropertyCommand` — generikus, reflection-en keresztül (text undo
  "Change Color", "Change Opacity" stb. property-név alapján)

A `SetPropertyCommand` általános: a reflection property-info-jából tudjuk,
melyik mezőt írjuk vissza.

### 6.4 Verification

- 5 művelet → 5× undo → eredeti állapot
- 5× undo → 5× redo → ismét végállapot
- Stress: 250 lépés → első 50 elveszik, többi visszavonható

---

## 7. fázis — Asset böngésző és file-dialog (≈ 1 hét)

**Output:** Az editor egy ImGui-panelben mutatja a `Resources/` (vagy
projektmappa) tartalmát ikonokkal, és a Save/Load Scene parancsok natív
file-dialógusokat nyitnak.

### 7.1 AssetDatabase

```cpp
// engine/assets/AssetDatabase.hpp
class AssetDatabase {
    std::unordered_map<Uuid, AssetMetadata> byGuid_;
    std::unordered_map<std::filesystem::path, Uuid> byPath_;
public:
    void scan(const std::filesystem::path& root);
    const AssetMetadata* find(const Uuid&) const;
    Texture* loadThumbnail(const Uuid&);  // 128×128 cache
};
```

A scan-elés a 3.5 `.meta`-fájl-konvenciót használja.

### 7.2 Cross-platform FileDialog

```cpp
// engine/platform/FileDialog.hpp
namespace fluxion::platform {
struct FileFilter { std::string name; std::vector<std::string> extensions; };
std::optional<std::filesystem::path> openFileDialog(
    const std::string& title,
    const std::vector<FileFilter>& filters);
std::optional<std::filesystem::path> saveFileDialog(...);
std::optional<std::filesystem::path> openFolderDialog(...);
}
```

Per-platform implementáció az **eddigi platform-mappákat** használja:

| Platform | Implementáció |
|---|---|
| Windows | `engine/core/windows/FileDialogWin.cpp` — `IFileOpenDialog` (Common Item Dialog) |
| macOS | `engine/core/macos/FileDialogMacOS.mm` — `NSOpenPanel` / `NSSavePanel` |
| Linux | `engine/core/linux/FileDialogLinux.cpp` — XDG portal D-Bus, fallback `zenity` |
| iOS / tvOS | UIDocumentPickerViewController (csak iOS) — tvOS-en N/A |
| Android | `Intent.ACTION_OPEN_DOCUMENT` JNI-n keresztül |
| Emscripten | `<input type="file">` JS-en át EM_JS-szel |

A platform-specifikus mappák már léteznek a Fluxion-ban (`engine/platform/winapi/`, `cocoa/`, `x11/`...) — csak hozzáadjuk az új source-okat.

### 7.3 Verification

- Save Scene → natív dialóg → JSON kiírva
- Load Scene → natív dialóg → scene visszatöltődik
- Asset böngészőben drag a sprite egy Inspector-mező fölé → texture beállítva

---

## 8. fázis — Hot reload (≈ 3-5 nap)

**Output:** A `Resources/` mappában fájl módosítása → 100-200 ms-on belül
a futó editor frissíti a textúrát/shadert/hangot, restart nélkül.

### 8.1 FileWatcher absztrakció

```cpp
// engine/storage/FileWatcher.hpp
class FileWatcher {
public:
    using Callback = std::function<void(const std::filesystem::path&, ChangeType)>;
    FileWatcher(const std::filesystem::path& root, Callback);
    ~FileWatcher();  // cleanup thread + handles
};
```

| Platform | Implementáció |
|---|---|
| Windows | `ReadDirectoryChangesW` (`engine/storage/windows/FileWatcherWin.cpp`) |
| macOS | `FSEventStream` (`engine/storage/macos/FileWatcherMacOS.mm`) |
| Linux | `inotify` |
| iOS / tvOS | dispatch_source (DISPATCH_SOURCE_TYPE_VNODE) — főként development-buildhez |
| Android | nincs (build-be kikapcsolva) — APK assetek read-only |
| Emscripten | nincs (browser FS read-only) |

### 8.2 Asset-újratöltés pipeline

```cpp
// engine/assets/Cache.cpp módosítás
void Cache::onFileChanged(const std::filesystem::path&) {
    // 1. ki van-e töltve cache-be — ha nem, ignore
    // 2. find loader by extension
    // 3. reload, in-place a meglévő Texture / Sound objektumokba (ne új pointer!)
    // 4. emit AssetReloaded esemény → UI frissítés
}
```

A meglévő `Texture` / `Sound` objektumok in-place frissítése kulcs:
nem akarjuk a scene-ben minden referenciát újrakötni.

### 8.3 Verification

- Editor fut → Sprites sample-rel → módosítjuk a `ball.png`-t Photoshop-ban →
  mentés → 200ms múlva a futó editor új textúrát mutatja.

---

## 9. fázis — Editor utility-k (≈ 1.5 hét)

### 9.1 Editor-kamera komponens

Új `EditorCamera` komponens (csak editor-build):
- 3D: free-fly (WASD + jobbklikk-look) + orbit (Alt+balklikk)
- 2D: pan (középső gomb) + zoom (kerék)
- Focus-on-selection (F-billentyű): viewport-középpont = bounding box
  középpont, távolság = bbox átmérő × 2.5
- Saját state: nem mentődik a scene fájlba

### 9.2 Debug overlay

Új `DebugDraw` namespace, immediate API:
```cpp
namespace fluxion::debug {
void drawLine(Vector3F a, Vector3F b, Color, float duration = 0.0f);
void drawBox(Box3F, Color);
void drawSphere(Vector3F center, float radius, Color);
void drawFrustum(const Camera&, Color);
}
```

Belső impl: a `ShapeRenderer` immediate-mode-jára épül, közös batch-cel.
Selection outline = AABB drawBox a kijelölés körül, sárga színnel.

### 9.3 Log konzol panel

A `engine/utils/Log.hpp` már megvan. Bővítés:
```cpp
class Log {
    using Subscriber = std::function<void(Level, const std::string&)>;
    static SubscriberId subscribe(Subscriber);
    static void unsubscribe(SubscriberId);
};
```

Az editor `LogPanel` egy ring-buffert tölt (max 10000 entry), szűrő
(level/source/text), auto-scroll, copy-to-clipboard.

### 9.4 Prefab rendszer

```cpp
// engine/scene/Prefab.hpp
class Prefab {
    Uuid prefabGuid;
    json serializedRoot;  // a 3. fázis formátuma
public:
    static Prefab createFromActor(Actor*);
    Actor* instantiate(Scene&, Vector3F position);
};
```

Prefab-fájlok `.fluxprefab` kiterjesztéssel a `Resources/`-ban,
GUID-da-zott meta-fájllal. Az Actor-példányok a forrás-prefab GUID-ját is
ismerik (override-vizualizációhoz későbbi fázisban).

### 9.5 Verification

- WASD-vel kameramozgás 3D-ben — jelenlegi sample módosítva
- F-billentyű → camera focus-ol a kijelölésre
- DebugDraw lines a sample-ben — 5 keresztező vonal renderelődik
- Log panel ImGui-ban — Log::log("test") → megjelenik

---

## 10. Cross-platform / cross-renderer teszt-mátrix

Minden fázis befejeztével a teljes mátrix futtatandó. Egy fázist csak
akkor zárunk le, ha minden cella ✅:

|  | D3D11 | OGL2 | OGL3/4 | OGL ES 2 | OGL ES 3 | Metal |
|---|---|---|---|---|---|---|
| Windows | ✅ | ✅ | ✅ | – | – | – |
| macOS | – | ✅ | ✅ | – | – | ✅ |
| Linux | – | ✅ | ✅ | – | – | – |
| iOS | – | – | – | ✅ | ✅ | ✅ |
| tvOS | – | – | – | – | ✅ | ✅ |
| Android | – | – | – | ✅ | ✅ | – |
| Emscripten | – | – | – | ✅ (WebGL1) | ✅ (WebGL2) | – |

A `samples/main.cpp`-be új `editor_smoke_test` parancs:
- Init → ImGui demo ablak → 60 frame render → screenshot a backbufferről
  → exit. Ezt CI-ben minden konfiguráción futtatni kell.

---

## 11. Rollout & migrációs sorrend

1. **Fázis 1 mergelése előtt**: új CI-job mindenikus konfigurációra (12+
   build-mátrix-cella). Minden meglévő samplet build-elni és lefuttatni
   ImGui-disabled módban — regresszió-mentes legyen.
2. **Fázis 2-3 mergelése előtt**: bevezetjük a `.meta`-fájlokat — egy egyszeri
   `tools/fluxion meta-generate` futás a `Resources/`-on, és commit a
   keletkezett .meta-fájlokról. Innentől minden új asset .meta-val születik.
3. **Editor mint külön projekt**: létrehozunk egy `editor/`
   könyvtárat `editor/editor.vcxproj`/`editor.xcodeproj`/`Makefile`-lel.
   Saját `main.cpp` és ImGui-Layout. A `samples/` a továbbiakban változatlan
   marad — a játék-sample-ek nem hivatkoznak az editorra.
4. **Editor-build flag**: `FLUXION_EDITOR_BUILD=1` define csak az editor
   project-ben. Reflection alap (typeName, factory) mindig forderül,
   de az `Inspector` UI-szerűsége csak editor-buildben.

---

## Becsült teljes idő

| Fázis | Munka | Bizonytalanság |
|---|---|---|
| 1. ImGui | 1.5 hét | ± 3 nap (per-platform DPI/font) |
| 2. Reflection | 1.5 hét | ± 2 nap |
| 3. Serialization + UUID | 1 hét | ± 3 nap (.meta migráció) |
| 4. Frame stepping | 1 hét | ± 4 nap (thread refaktor kockázatos) |
| 5. Picking + gizmók | 1.5 hét | ± 4 nap |
| 6. Undo/redo | 0.5 hét | ± 1 nap |
| 7. Asset böngésző + dialógus | 1 hét | ± 3 nap (per-platform dialog) |
| 8. Hot reload | 0.5 hét | ± 2 nap |
| 9. Utility-k | 1.5 hét | ± 3 nap |
| **Összesen** | **~10 hét egy fő munkára** | ± 2-3 hét összes |

A kritikus út fázis 1-4 (5 hét). Innentől a 5-9 részben párhuzamosítható,
ha több fő dolgozik rajta (pl. fázis 6 függetlenül futhat 5 mellett).

---

## Mire kell figyelni mindenhol

- **API-stabilitás**: minden új publikus header `FLUXION_API` macro-val,
  hogy DLL-buildbe is lehessen exportálni — bár a Fluxion most static lib,
  jövőbeli rugalmasságért érdemes betartani.
- **Allokáció minimum a render-szálon**: az ImGui per-frame copy-elhet
  vertex-buffert, de saját kódban kerüljük az `std::string`-allokációt
  hot-pathon. ImGui io.MemAlloc/Free-be kötjük a Fluxion allokátorát ha van.
- **DPI awareness**: minden ImGui-panel pixelméret szorzandó a
  `Window::getContentScale()`-lel. iOS Retina, Windows HiDPI, macOS
  Retina egyaránt fontos.
- **Undo-állapot ne sérüljön**: minden command kétirányú legyen
  (apply / revert) és NE függjön a "current state"-től, mert az
  undo-stack-en már elképzelhető, hogy más állapotban hívják meg.
- **Editor savestate ne kerüljön a játék-szériába**: az editor-state
  (selection, kamera-pozíció, panel-layout) saját `editor.preferences.json`-ba
  megy a user-mappába, nem a scene-fájlba.

---

A terv kész. Implementáláshoz a 0. fejezetben rögzített kereszt-fázis
döntéseket fogadjuk el először, aztán a fázisokat sorban kezdjük.
