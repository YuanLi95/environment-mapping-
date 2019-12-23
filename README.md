
在资源中使用aiTextureType_AMBIENT作为reflection map，在model.h代码中添加处理:

```cpp
// 获取Reflection 注意: AssImp对Reflection支持不好 所以这里采用ambient_map
// 除了这里的代码 还需要修改对应的obj文件
std::vector<Texture> reflectionTexture;
this->processMaterial(materialPtr, sceneObjPtr, aiTextureType_AMBIENT, reflectionTexture);
textures.insert(textures.end(), reflectionTexture.begin(), reflectionTexture.end());
```
同时在mesh.h中也要添加相应的处理，在片元着色器中修改为：

```cpp
#version 330 core

in vec3 FragNormal;
in vec3 FragPos;
in vec2 TextCoord;

uniform samplerCube envText;	// 环境纹理
uniform sampler2D texture_diffuse0;
uniform sampler2D specular_diffuse0;
uniform sampler2D texture_reflection0; // 反射map
uniform vec3 cameraPos;

out vec4 color;


void main()
{
	vec4	diffuseColor = texture(texture_diffuse0, TextCoord);
	float	relefctIntensity = texture(texture_reflection0, TextCoord).r; 
	vec4	reflectColor = vec4(0.0, 0.0, 0.0, 0.0);
	if(relefctIntensity > 0.1) // 决定是否开启
	{
		vec3 viewDir = normalize(FragPos - cameraPos); 
		vec3 reflectDir = reflect(viewDir, normalize(FragNormal));
		reflectColor = texture(envText, reflectDir) * relefctIntensity;	// 使用反射向量采样环境纹理 使用强度系数控制
	}
	color = diffuseColor + reflectColor;
}
```
这里通过读取reflection map，采样纹理后，获取一个强度系数，根据强度系数来决定是否开启refleciton map，如果开启则输出颜色为diffuse map和reflection map的混合结果。

渲染时只输出diffuse map或者reflection map，以及最终得到的效果，对比如下图所示：

![reflection map](http://img.blog.csdn.net/20160916212116711)

对头部进行放大，我们看到了反射贴图的效果：

![reflection part](http://img.blog.csdn.net/20160916212243312)

反射贴图为模型的表面增加了环境成分，显得更加逼真，而且模型在移动过程中，贴图能够动态变换，反映位置的改变。


# 最后的说明
在实现本节内容过程中，需要注意，使用反射或者折射时需要提供物体表面法向量，不仅需要修改顶点属性数据，还要修改对应的顶点着色器，如果修改错误，可能产生错误效果如下图：

![这里写图片描述](http://img.blog.csdn.net/20160916212650396)

从图中结果，可以看到渲染基本的图形出错，可以立马联想到顶点属性数据配置出错。

同时在加载模型，使用reflection map时，天空包围盒使用的纹理单元，要更新为3， 因为前面加载了其他的specular map、diffuse map、reflection map:

```cpp
glActiveTexture(GL_TEXTURE3);
glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.getTextId());
glUniform1i(glGetUniformLocation(shader.programId, "envText"), 3); 
```
如果没有更新这个配置，导致天空包围盒的纹理单元被其他纹理占用，将发生错误，例如可能的错误结果如下：

![refletion map error](http://img.blog.csdn.net/20160916213037525)

关于环境纹理贴图，还有一项技术称为dynamic environment mapping。通过借助framebuffer,为每个物体渲染6个包含场景中其他物体的纹理，从而构建一个环境纹理，然后实行贴图。这种动态贴图方式，由于在framebuffer中要为每个物体执行6次场景渲染，在保持较好性能开销下使用它需要很多技巧，没有这里介绍的天空包围盒这么容易使用。在后面时间充足时，可以学习下这个技术。

# environment-mapping-
