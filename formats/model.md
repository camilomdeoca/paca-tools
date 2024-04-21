# Binary Model Format for Paca Engine

## Extension: `.pmdl`

### Header

| Size    | Format       | Description                        |
| ------- | ------------ | ---------------------------------- |
| 4 bytes | ascii text   | "PMDL" to identify type of file    |
| 4 bytes | unsigned int | Format version                     |
| 4 bytes | unsigned int | Number of meshes the model has     |
| 4 bytes | unsigned int | Length of model name               |
| n bytes | text         | Model name                         |

### Each mesh

| Size    | Format       | Description                         |
| ------- | ------------ | ----------------------------------- |
| 4 bytes | unsigned int | The vertex type [¹](#vertex-types)  |
| 4 bytes | unsigned int | The number of vertices              |
| 4 bytes | unsigned int | The index type [²](#index-types)    |
| 4 bytes | unsigned int | The number of indices               |
| 4 bytes | unsigned int | The number of bones (if 0 no bones) |
| 4 bytes | unsigned int | The number of animations            |
| 4 bytes | unsigned int | The Length of the material name     |
| n bytes | text         | Material name                       |

### Each animation

| Size    | Format       | Description                         |
| ------- | ------------ | ----------------------------------- |
| 4 bytes | unsigned int | Animation name length               |
| n bytes | unsigned int | Animation name                      |

### Each bone

| Size     | Format       | Description                             |
| -------- | ------------ | --------------------------------------- |
| 4 bytes  | int          | The bone id                             |
| 64 bytes | 16 floats    | Row major offset 4x4 matrix of the bone |
| 4 bytes  | unsigned int | Length of bone name                     |
| n bytes  | text         | Bone name                               |

#### Now Vertex data

It would be `size_of_vertex_in_bytes * number_of_vertices` bytes of vertex data.

#### Now Indices

If present, `size_of_index_in_bytes * number_of_indices` bytes of indices.

##### Vertex Types

| Value | Description                                         |
| ----- | --------------------------------------------------- |
| 1     | vec3 position, vec3 normal, vec2 textureCoordinates |

##### Index Types

| Value | Description          |
| ----- | -------------------- |
| 0     | No indices           |
| 1     | 32bit unsigned int   |

### Full File

```
| Model header
| `mesh_count` times
    | Mesh header
    | `bone_count` times
        | Bone Subheader
    | vertex data
    | index data
```
