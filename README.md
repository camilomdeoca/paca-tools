# Tools for the paca engine

## Build

```
cmake . && cmake --build .
```

## Tools

### Model to paca model converter

Generates engine's model and material files from model file.

Usage:

```
./build/to-paca-format <input_model>
```

### Library to read paca models and materials

Reads the files converted from the [paca model converter tool](#model-to-paca-model-converter).
