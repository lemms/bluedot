# bluedot
bluedot planet texture generator

#Usage:

```
bluedot 1.0
Allowed options:
  -h [ --help ]         Produce help message
  -i [ --input ] arg    Input configuration file
  -o [ --output ] arg   Output file
```

bluedot accepts an input configuration file of type .xml, .json, .ini, or .info.
Refer to the Configuration Format section for details on the formatting of the input file.

bluedot outputs a .ppm file with the planet texture.

#Configuration Format:

The configuration file is a hierarchical file containing these nodes:

```
--> map
  |
  |--> width : <width>
  |
  |--> height : <height>
  |
  |--> [seed : <seed>]
  |
  |--> layers
  | | 
  | |--> layer
  | | |
  | | |--> name : <layer name>
  | | |
  | | |--> [channels : <number of channels>]
  | |
  | |--> layer
  | | |
  | | |--> name: <layer name>
  | | |
  | | |--> [channels : <number of channels>]
  | |
  |
  |--> operators
    |
    |--> operator
    | |
    | |--> type : <operator type>
    | |
    | |--> <operator type dependent arguments>
           <see the section Operators for details>
```

#Operators:

Operators in bluedot are applied in the top down order they are listed in the file.
bluedot has two kinds of operator:
- unary operators act on a single layer.
- binary operators act on two layers, layer0 and layer1, overwriting the data in layer0.

All available operators are listed with their required and optional parameters in the section Operator List.

Typically, operations will be applied to all channels of a layer, with an optional scale, offset, and per-channel multiplier.

Most unary operations will use the following formula:

```
layer[c] += op(multiplier[c] * scale + offset)
```

and most binary operations will use the following formula

```
layer0[c] += op(layer1[c] * multiplier[c] * scale + offset)
```

A mask layer may be specified in which case most operations will use the following formula:

```
layer[c] += op(mask[c] * multiplier[c] * scale + offset)
```

Multipliers are specified in the configuration file as follows:

```
--> multiplier
 |
 |--> [format : <format>]
 |--> a : <a>
 |--> r : <r>
 |--> g : <g>
 |--> b : <b>
```

The multiplier format may be:
Real for real numbers
Char for values in the range [0, 256)
Percent for values in the range [0, 100)

#Operator List:

AlphaBlendOperator
Performs alpha blending between two layers.
```
- layer0 : <name of layer0>
- layer1 : <name of layer1>
- [mask : <name of mask layer>]
- [multiplier : <per channel multiplier>]
- [scale : <scale>]
- [offset : <offset>]
```

AlphaToColorOperator
Sets all color channels to the alpha value in a layer
```
- layer : <name of layer>
- [multiplier : <per channel multiplier>]
- [scale : <scale>]
- [offset : <offset>]
```

ColorToAlphaOperator
Uses lightness to set the alpha channel of a layer
```
- layer : <name of layer>
- [multiplier : <per channel multiplier>]
- [scale : <scale>]
- [offset : <offset>]
```

FBMOperator
Applies fractional Brownain motion to a layer
```
- layer : <name of layer>
- [octaves : <number of octaves>]
- [exponent : <exponent>]
- [multiplier : <per channel multiplier>]
- [scale : <scale>]
- [offset : <offset>]
```

FillOperator
Fills a layer with the color specified
```
- layer : <name of layer>
- [multiplier : <per channel multiplier>]
- [scale : <scale>]
- [offset : <offset>]
```

GradientOperator
Computes the grad of the alpha channel of a layer and storing the result in the R and G channels
```
- layer : <name of layer>
- [multiplier : <per channel multiplier>]
- [scale : <scale>]
- [offset : <offset>]
```

GreaterThanOperator
Checks whether a sample has a value greater than level.
If it is less than level, it is either set to 0 or clamped to the level depending on whether Clamp is set.
```
- layer : <name of layer>
- [mode : {"Clamp", "Zero"}]
- [level : <per channel level to compare with>]
```

LessThanOperator
Checks whether a sample has a value less than level.
If it is greater than level, it is either set to 0 or clamped to the level depending on whether Clamp is set.
```
- layer : <name of layer>
- [mode : {"Clamp", "Zero"}]
- [level : <per channel level to compare with>]
```

MultiplyOperator
Multiplies layer0 and layer1 and stores the result in layer0
```
- layer0 : <name of layer0>
- layer1 : <name of layer1>
- [multiplier : <per channel multiplier>]
- [scale : <scale>]
- [offset : <offset>]
```

SwapOperator
Swaps the values of layer0 and layer1
```
- layer0 : <name of layer0>
- layer1 : <name of layer1>
```

#Examples:


##islands.json:

This file generates a set of green islands in a blue ocean.

```
{
    "map" :
    {
        "seed" : 1024,
            "width" : 2048,
            "height" : 2048,
            "layers" :
            {
                "layer" :
                {
                    "name" : "base",
                    "channels" : 4
                },
                "layer" :
                {
                    "name" : "islands",
                    "channels" : 4
                }
            },
            "operators" :
            {
                "operator" :
                {
                    "type" : "FillOperator",
                    "layer" : "base",
                    "multiplier" : 
                    {
                        "format" : "Char",
                        "a" : 255,
                        "r" : 46,
                        "g" : 82,
                        "b" : 140
                    }
                },
                "operator" :
                {
                    "type" : "FBMOperator",
                    "layer" : "islands",
                    "octaves" : 7,
                    "exponent" : 2.2,
                    "multiplier" : 
                    {
                        "format" : "Char",
                        "a" : 255,
                        "r" : 36,
                        "g" : 109,
                        "b" : 47
                    },
                    "scale" : 0.3,
                    "offset" : -0.7
                },
                "operator" :
                {
                    "type" : "FBMOperator",
                    "layer" : "islands",
                    "octaves" : 3,
                    "exponent" : 2.0,
                    "multiplier" : 
                    {
                        "format" : "Char",
                        "a" : 255,
                        "r" : 36,
                        "g" : 109,
                        "b" : 47
                    },
                    "scale" : 0.6,
                    "offset" : 0.0
                },
                "operator" :
                {
                    "type" : "LessThanOperator",
                    "layer" : "islands",
                    "mode" : "Clamp",
                    "level" : 
                    {
                        "format" : "Char",
                        "a" : 255,
                        "r" : 36,
                        "g" : 109,
                        "b" : 47
                    }
                },
                "operator" :
                {
                    "type" : "GreaterThanOperator",
                    "layer" : "islands",
                    "level" :
                    {
                        "a" : 0,
                        "r" : 0,
                        "g" : 0,
                        "b" : 0
                    }
                },
                "operator" :
                {
                    "type" : "AlphaBlendOperator",
                    "layer0" : "base",
                    "layer1" : "islands"
                }
            }
    }
}
```

