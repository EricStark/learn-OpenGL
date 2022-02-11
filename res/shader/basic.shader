#shader vertex
#version 330 core

layout(location = 0) in vec3 position;    // 顶点坐标为0
layout(location = 2) in vec2 texCoord;    // 纹理坐标为2
out vec2 TexCoord;                        // 向片段着色器输出纹理坐标
//uniform mat4 transform;					  // 变换矩阵

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	// mvp
	gl_Position = projection * view * model * vec4(position, 1.0f); //注意乘的顺序要相反 即 pvm
	TexCoord = vec2(texCoord.x, 1.0 - texCoord.y);
};

#shader fragment
#version 330 core

uniform sampler2D ourTexture1;// 纹理1
uniform sampler2D ourTexture2;// 纹理2
out vec4 color;
in vec2 TexCoord;// 纹理坐标

void main()
{
	// mix 第三个参数表示插值，0则返回第一个纹理，1则返回第二个纹理
	color = mix(texture(ourTexture1, TexCoord), texture(ourTexture2, TexCoord), 0.2);
};