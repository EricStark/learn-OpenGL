#shader vertex
#version 330 core

layout(location = 0) in vec3 position;    // ��������Ϊ0
layout(location = 2) in vec2 texCoord;    // ��������Ϊ2
out vec2 TexCoord;                        // ��Ƭ����ɫ�������������
//uniform mat4 transform;					  // �任����

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	// mvp
	gl_Position = projection * view * model * vec4(position, 1.0f); //ע��˵�˳��Ҫ�෴ �� pvm
	TexCoord = vec2(texCoord.x, 1.0 - texCoord.y);
};

#shader fragment
#version 330 core

uniform sampler2D ourTexture1;// ����1
uniform sampler2D ourTexture2;// ����2
out vec4 color;
in vec2 TexCoord;// ��������

void main()
{
	// mix ������������ʾ��ֵ��0�򷵻ص�һ������1�򷵻صڶ�������
	color = mix(texture(ourTexture1, TexCoord), texture(ourTexture2, TexCoord), 0.2);
};