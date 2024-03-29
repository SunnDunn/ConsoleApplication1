# version 330 core

layout(location = 0) in vec3 aPos;
//layout(location = 2) in vec2 aTex;

//out vec2 texCoord;

uniform mat4 transform;
uniform mat4 projection;
//uniform mat4 view;

void main()
{
	gl_Position = projection  * transform * vec4(aPos, 1.0);

	texCoord = aTex;
}