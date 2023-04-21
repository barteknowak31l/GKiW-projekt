#version 330
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;



uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


uniform float minHeight;
uniform float maxHeight;

out vec4 Color;
out vec2 Tex;
out vec3 Normal;
out vec3 LocalPos;
out vec3 FragPos;

void main()
{
	// alternatywne kolorowanie na bazie wysokosci - mozna uzyc gdy nie ma tekstury
	float deltaHeight = maxHeight - minHeight;
	float HeightRatio = (aPos.y - minHeight) / deltaHeight;
	float c = HeightRatio * 0.8 + 0.2;
	Color = vec4(c,c,c,1.0);

	Tex = aTexCoords;

	LocalPos = aPos;
	FragPos =  vec3(model * vec4(aPos, 1.0));

    Normal = mat3(transpose(inverse(model))) * aNormal;

    gl_Position = projection *view * model * vec4(aPos, 1.0);
}
