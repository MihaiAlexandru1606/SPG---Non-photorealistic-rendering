#version 410

layout(location = 0) in vec2 texture_coord;

uniform sampler2D textureImage;
uniform ivec2 screenSize;
uniform int flipVertical;

// 0 - original
// 1 - grayscale
// 2 - blur
uniform int outputMode = 2;

// Flip texture horizontally when
vec2 textureCoord = vec2(texture_coord.x, (flipVertical != 0) ? 1 - texture_coord.y : texture_coord.y);

layout(location = 0) out vec4 out_color;

vec4 grayscale()
{
	vec4 color = texture(textureImage, textureCoord);
	float gray = 0.21 * color.r + 0.71 * color.g + 0.07 * color.b; 
	return vec4(gray, gray, gray,  0);
}

vec4 my_grayScale()
{
	vec4 color = texture(textureImage, textureCoord);
	float gray = 1.0f * (color.r +  color.g + color.b) / 3;

	return vec4(gray, gray, gray,  0);
}


vec4 blur(int blurRadius)
{
	vec2 texelSize = 1.0f / screenSize;
	vec4 sum = vec4(0);
	for(int i = -blurRadius; i <= blurRadius; i++)
	{
		for(int j = -blurRadius; j <= blurRadius; j++)
		{
			sum += texture(textureImage, textureCoord + vec2(i, j) * texelSize);
		}
	}
		
	float samples = pow((2 * blurRadius + 1), 2);
	return sum / samples;
}


vec4 frameValues[9];
float arr[9];

void bubbleSort()
{
    bool swapped = true;
    int j = 0;
    float tmp;
    for (int c = 0; c < 3; c--)
    {
        if (!swapped)
            break;
        swapped = false;
        j++;
        for (int i = 0; i < 3; i++)
        {
            if (i >= 3 - j)
                break;
            if (arr[i] > arr[i + 1])
            {
                tmp = arr[i];
                arr[i] = arr[i + 1];
                arr[i + 1] = tmp;
                swapped = true;
            }
        }
    }
}

vec4 median(int blurRadius)
{
	vec2 texelSize = 1.0f / screenSize;
	vec4 sum = vec4(0);

	int s = 0;
	float f;
	for(int i = -blurRadius; i <= blurRadius; i++)
	{
		for(int j = -blurRadius; j <= blurRadius; j++)
		{
			vec4 color = texture(textureImage, textureCoord + vec2(i, j) * texelSize);
			//float gray = 0.21 * color.r + 0.71 * color.g + 0.07 * color.b;
			arr[s] = color.r;
			
			s++;
		}
	}
	
	bubbleSort();
	return vec4(arr[4],arr[4], arr[4], 0);
}

void main()
{
	switch (outputMode)
	{
		case 1:
		{
			out_color = median(3);
			break;
		}

		case 2:
		{
			out_color = blur(3);
			break;
		}

		case 3:
		{
			out_color = my_grayScale();
			break;
		}

		default:
			out_color = texture(textureImage, textureCoord);
			break;
	}
}