# Binary Material Format for Paca Engine

## Extension: `.pmat`

### Header

| Size    | Format       | Description                            |
| ------- | ------------ | -------------------------------------- |
| 4 bytes | ascii text   | "PMAT" to identify type of file        |
| 4 bytes | unsigned int | Format version                         |
| 4 bytes | unsigned int | Texture Count                          |
| 4 bytes | unsigned int | Length of material name                |
| n bytes | text         | Material name                          |

### Each texture

| Size    | Format       | Description                            |
| ------- | ------------ | -------------------------------------- |
| 4 bytes | unsigned int | Type of texture (¹)[#types-of-texture] |
| 4 bytes | unsigned int | Length of filepath of texture          |
| n bytes | text         | Filepath of texture                    |

##### Types of Texture

| Value | Description |
| ----- | ----------- |
| 0     | None        |
| 1     | diffuse     |
| 2     | specular    |

