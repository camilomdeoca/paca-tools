# Binary Animation Format for Paca Engine

## Extension: `.pani`

### Header

| Size    | Format       | Description                        |
| ------- | ------------ | ---------------------------------- |
| 4 bytes | ascii text   | "PANI" to identify type of file    |
| 4 bytes | unsigned int | Format version                     |
| 4 bytes | unsigned int | Number of bone keyframes           |
| 4 bytes | float        | Duration of the animation in ticks |
| 4 bytes | float        | Ticks per second                   |
| 4 bytes | unsigned int | Length of animation name           |
| n bytes | text         | Animation name                     |

### Each animation bone

| Size    | Format       | Description                        |
| ------- | ------------ | ---------------------------------- |
| 4 bytes | unsigned int | The number of position keyframes   |
| 4 bytes | unsigned int | The number of rotation keyframes   |
| 4 bytes | unsigned int | The number of scaling keyframes    |

### Each position keyframe

| Size     | Format     | Description                         |
| -------- | ---------- | ----------------------------------- |
| 4 bytes  | float      | Time of the keyframe                |
| 12 bytes | float vec3 | The position                        |

### Each rotation keyframe

| Size     | Format     | Description                         |
| -------- | ---------- | ----------------------------------- |
| 4 bytes  | float      | Time of the keyframe                |
| 16 bytes | float quat | The quaternion of the rotation      |

### Each scaling keyframe

| Size     | Format     | Description                         |
| -------- | ---------- | ----------------------------------- |
| 4 bytes  | float      | Time of the keyframe                |
| 12 bytes | float vec3 | The scaling                         |

