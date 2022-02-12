#shader vertex
#version 330 core
layout(location = 0) in vec3 position;    // ��������Ϊ0
layout(location = 1) in vec3 normal;      // ÿ����ķ�����
layout(location = 2) in vec2 texCoords;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;
void main()
{
	Normal = mat3(transpose(inverse(model))) * normal;
	TexCoords = texCoords;
	FragPos = vec3(model * vec4(position, 1.0f));//�õ�ÿ�������������������λ�� Ȼ���Ƭ����ɫ������������
	// mvp
	gl_Position = projection * view * model * vec4(position, 1.0f); //ע��˵�˳��Ҫ�෴ �� pvm
};
#shader fragment
#version 330 core
struct Material
{
	sampler2D diffuse;//��������ͼ
	sampler2D specular;//������ͼ
	float shininess;
};
//���Զ����
//struct Light
//{
//	vec3 direction;
//	vec3 ambient;
//	vec3 diffuse;
//	vec3 specular;
//};
//���Ե��
//struct Light
//{
//	vec3 position;
//	vec3 ambient;
//	vec3 diffuse;
//	vec3 specular;
//	float constant;//˥��������
//	float linear;//˥��һ����
//	float quadratic;//˥��������
//};
//���Ծ۹�
struct Light
{
	vec3 position;
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float cutOff;//�й��
	float outerCutOff;
	float constant;//˥��������
	float linear;//˥��һ����
	float quadratic;//˥��������
};
uniform Light light;
uniform Material material;//����
uniform vec3 lightPos;
uniform vec3 viewPos;//�۲�����
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;//������ͼ
out vec4 color;
void main()
{
	//////////////////���Զ����//////////////////
	//// Ambient
	//vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
	//// Diffuse 
	//vec3 norm = normalize(Normal);
	//vec3 lightDir = normalize(-light.direction);
	//float diff = max(dot(norm, lightDir), 0.0);
	//vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
	//// Specular
	//vec3 viewDir = normalize(viewPos - FragPos);
	//vec3 reflectDir = reflect(-lightDir, norm);
	//float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	//vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
	//color = vec4(ambient + diffuse + specular, 1.0f);
	////////////////////���Ե��//////////////////
	//// Ambient
	//vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
	//// Diffuse 
	//vec3 norm = normalize(Normal);
	//vec3 lightDir = normalize(light.position - FragPos);//--�����Ե��Դ
	//float diff = max(dot(norm, lightDir), 0.0);
	//vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
	//// Specular
	//vec3 viewDir = normalize(viewPos - FragPos);
	//vec3 reflectDir = reflect(-lightDir, norm);
	//float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	//vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
	//float distance = length(light.position - FragPos);
	//float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	//ambient *= attenuation;
	//diffuse *= attenuation;
	//specular *= attenuation;
	//color = vec4(ambient + diffuse + specular, 1.0f);
	//////////////////���Ծ۹⣨�ޱ�Եƽ�����ȴ���//////////////////
	//vec3 lightDir = normalize(light.position - FragPos);
	//// Check if lighting is inside the spotlight cone
	//float theta = dot(lightDir, normalize(-light.direction));
	//if (theta > light.cutOff) // Remember that we're working with angles as cosines instead of degrees so a '>' is used.
	//{
	//	// Ambient
	//	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
	//	// Diffuse 
	//	vec3 norm = normalize(Normal);
	//	float diff = max(dot(norm, lightDir), 0.0);
	//	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
	//	// Specular
	//	vec3 viewDir = normalize(viewPos - FragPos);
	//	vec3 reflectDir = reflect(-lightDir, norm);
	//	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	//	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
	//	// Attenuation
	//	float distance = length(light.position - FragPos);
	//	float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	//	// ambient  *= attenuation;  // Also remove attenuation from ambient, because if we move too far, the light in spotlight would then be darker than outside (since outside spotlight we have ambient lighting).
	//	diffuse *= attenuation;
	//	specular *= attenuation;
	//	color = vec4(ambient + diffuse + specular, 1.0f);
	//}
	//else    // else, use ambient light so scene isn't completely dark outside the spotlight.
	//	color = vec4(light.ambient * vec3(texture(material.diffuse, TexCoords)), 1.0f);

	//////////////////���Ծ۹⣨��Եƽ�����ȴ���//////////////////
	// ambient
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
	// diffuse 
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
	// specular
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
	// attenuation
	float distance = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	// check if lighting is inside the spotlight cone
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
	diffuse *= intensity;
	specular *= intensity;
	vec3 result = ambient + diffuse + specular;
	color = vec4(result, 1.0);
};