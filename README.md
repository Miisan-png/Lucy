# Lucy

Lightweight Entity Component System Header-only, zero dependencies, sparse-set based.

Built for [Kurumi Engine](https://github.com/Miisan-png/Kurumi).

## Usage

```cpp
#include <lucy/lucy.hpp>

struct Position { float x, y; };
struct Velocity { float vx, vy; };

lucy::Registry reg;

auto e = reg.create();
reg.add<Position>(e, 10.0f, 20.0f);
reg.add<Velocity>(e, 1.0f, 0.0f);

reg.each<Position, Velocity>([](lucy::Entity e, Position& p, Velocity& v) {
    p.x += v.vx;
    p.y += v.vy;
});

reg.destroy(e);
```

## Integration

```cmake
add_subdirectory(lucy)
target_link_libraries(your_target lucy)
```

## License

MIT
