#include "shader_skybox.h"
#include "shader_generator.h"

void SkyBoxConfig::parse_from_material_keyvalues( const BSPMaterial *mat )
{
}

SkyBoxSpec::SkyBoxSpec() :
        ShaderSpec( "SkyBox", Filename( "shaders/stdshaders/skybox.vert.glsl" ),
                Filename( "shaders/stdshaders/skybox.frag.glsl" ) )
{
}

void SkyBoxSpec::setup_permutations( ShaderPermutations &result,
	const BSPMaterial *mat,
	const RenderState *rs,
	const GeomVertexAnimationSpec &anim,
	BSPShaderGenerator *generator )
{
	ShaderSpec::setup_permutations( result, mat, rs, anim, generator );

        result.add_input( ShaderInput( "skyboxSampler", generator->get_identity_cubemap() ) );
}

PT( ShaderConfig ) SkyBoxSpec::make_new_config()
{
        return new SkyBoxConfig;
}