/**
 * PANDA3D BSP LIBRARY
 * 
 * Copyright (c) Brian Lach <brianlach72@gmail.com>
 * All rights reserved.
 *
 * @file ambient_probes.h
 * @author Brian Lach
 * @date August 03, 2018
 */

#ifndef AMBIENT_PROBES_H
#define AMBIENT_PROBES_H

#include <pvector.h>
#include <weakNodePath.h>
#include <aa_luse.h>
#include <pta_LVecBase3.h>
#include <colorAttrib.h>
#include <texGenAttrib.h>
#include <lightRampAttrib.h>
#include <cullableObject.h>
#include <shaderAttrib.h>
#include <updateSeq.h>

#include <unordered_map>
#include <bitset>

#include "kdtree/KDTree.h"

#include "config_bsp.h"

class BSPLoader;
struct dleafambientindex_t;
struct dleafambientlighting_t;
class cubemap_t;

enum
{
        LIGHTTYPE_SUN           = 0,
        LIGHTTYPE_POINT         = 1,
        LIGHTTYPE_SPHERE        = 2,
        LIGHTTYPE_SPOT          = 3,
};

struct ambientprobe_t : public ReferenceCount
{
        int leaf;
        LPoint3 pos;
        PTA_LVecBase3 cube;
        NodePath visnode;
};

struct light_t : public ReferenceCount
{
        int id;
        int leaf;
        int type;
        LVector4 direction;
        LPoint3 pos;
        LVector3 color;
        LVector4 falloff;
        LVector4 falloff2;
        LVector4 falloff3;

        // If the light is potentially visible, updated each frame.
        LVector4 eye_pos;
        LVector4 eye_direction;
};

// Max stationary lights
#define MAX_ACTIVE_LIGHTS 4
// Max lights we could be dimming
#define MAX_TOTAL_LIGHTS 8

#define LIGHTING_UNINITIALIZED -1

#ifndef CPPPARSER
class CNodeShaderInput : public TypedReferenceCount
{
        DECLARE_CLASS( CNodeShaderInput, TypedReferenceCount );

public:
        // This is the ambient cube actually used by the shader
        PTA_LVecBase3 ambient_cube;
        // This is a storage of the ambient cube pre-boosting,
        // so we can correctly time average the ambient cube,
        // and then boost.
        LVector3 boxcolor[6];
        LVector3 boxcolor_boosted[6];
        bool ambient_boost;

        PTA_int light_count;
        PTA_int light_type;
        PTA_LMatrix4f light_data;
        PTA_LMatrix4f light_data2;

        PN_stdfloat lighting_time;
        PTA_int light_ids;

        // The BSP level context for this node.
        // If the node's level context is invalid, we have light data
        // from a previous level, and should not attempt to fade them
        // out, as they have been deleted.
        UpdateSeq level_context;

        ambientprobe_t *amb_probe;
        cubemap_t *cubemap;
        PT( Texture ) cubemap_tex;
        bool cubemap_changed;
        pvector<light_t *> locallights;
        int sky_idx;
        std::bitset<0xFFF> occluded_lights;

        CPT( RenderState ) state_with_input;
        CPT( TransformState ) last_transform;

        int active_lights;

        INLINE void copy_needed( const CNodeShaderInput *other )
        {
                light_count.set_data( other->light_count.get_data() );
                light_ids.set_data( other->light_ids.get_data() );
                light_data.set_data( other->light_data.get_data() );
                light_type.set_data( other->light_type.get_data() );
                active_lights = other->active_lights;
        }

        INLINE CNodeShaderInput( const CNodeShaderInput *other ) :
                TypedReferenceCount()
        {
                copy_needed( other );
        }

        CNodeShaderInput() :
                TypedReferenceCount()
        {
                ambient_cube = PTA_LVecBase3::empty_array( 6 );

                light_count = PTA_int::empty_array( 1 );
                light_type = PTA_int::empty_array( MAX_TOTAL_LIGHTS );
                light_data = PTA_LMatrix4f::empty_array( MAX_TOTAL_LIGHTS );
                light_data2 = PTA_LMatrix4f::empty_array( MAX_TOTAL_LIGHTS );
                light_ids = PTA_int::empty_array( MAX_TOTAL_LIGHTS );
                amb_probe = nullptr;
                cubemap = nullptr;
                state_with_input = nullptr;
                last_transform = nullptr;
                cubemap_tex = new Texture( "cubemap_image" );
		cubemap_tex->setup_cube_map( 32, Texture::T_unsigned_byte, Texture::F_rgb8 );
                cubemap_tex->clear_image();
                sky_idx = -1;
                active_lights = 0;
                ambient_boost = false;
                memset( boxcolor, 0, sizeof( LVector3 ) * 6 );
                memset( boxcolor_boosted, 0, sizeof( LVector3 ) * 6 );

                lighting_time = LIGHTING_UNINITIALIZED;
        }

        CNodeShaderInput( const CNodeShaderInput &other ) :
                TypedReferenceCount(),
                lighting_time( other.lighting_time ),
                level_context( other.level_context ),
                amb_probe( other.amb_probe ),
                locallights( other.locallights ),
                sky_idx( other.sky_idx ),
                active_lights( other.active_lights ),
                occluded_lights( other.occluded_lights ),
                cubemap_tex( other.cubemap_tex ),
                state_with_input( other.state_with_input ),
                last_transform( other.last_transform )
        {
                //ambient_cube = PTA_LVecBase3::empty_array( 6 );
                //light_count = PTA_int::empty_array( 1 );
                //light_type = PTA_int::empty_array( MAX_TOTAL_LIGHTS );
                //light_data = PTA_LMatrix4f::empty_array( MAX_TOTAL_LIGHTS );
                //light_ids = PTA_int::empty_array( MAX_TOTAL_LIGHTS );

                light_type.set_data( other.light_type.get_data() );
                ambient_cube.set_data( other.ambient_cube.get_data() );
                light_type.set_data( other.light_type.get_data() );
                light_data.set_data( other.light_data.get_data() );
                light_data2.set_data( other.light_data2.get_data() );
                light_ids.set_data( other.light_ids.get_data() );
                light_count.set_data( other.light_count.get_data() );
        }
};
#else
class CNodeShaderInput;
#endif

class EXPCL_PANDABSP AmbientProbeManager
{
public:
        AmbientProbeManager();
        AmbientProbeManager( BSPLoader *loader );

        void process_ambient_probes();

	const RenderState *update_node( PandaNode *node, CPT( TransformState ) net_ts, bool should_update = true );

        void load_cubemaps();

        template<class T>
        T find_closest_in_kdtree( KDTree *tree, const LPoint3 &pos,
                                  const pvector<T> &items );

        template<class T>
        T find_closest_n_in_kdtree( KDTree *tree, const LPoint3 &pos,
                                  const pvector<T> &items, int n );

        void cleanup();

        //INLINE KDTree *get_light_kdtree() const
        //{
        //        return _light_kdtree;
        //}
        INLINE KDTree *get_envmap_kdtree() const
        {
                return _envmap_kdtree;
        }
        INLINE KDTree *get_probe_kdtree( int leaf ) const
        {
                int itr = _probe_kdtrees.find( leaf );
                if ( itr == -1 )
                        return nullptr;
                return _probe_kdtrees.get_data( itr );
        }
        INLINE const pvector<PT( cubemap_t )> &get_cubemaps()
        {
                return _cubemaps;
        }

        INLINE light_t *get_sunlight() const
        {
                return _sunlight;
        }

public:

        void xform_lights( const TransformState *cam_trans );

private:
        INLINE bool is_sky_visible( const LPoint3 &point );
        INLINE bool is_light_visible( const LPoint3 &point, const light_t *light );

private:
        BSPLoader *_loader;

        // NodePaths to be influenced by the ambient probes.
        SimpleHashMap<int, pvector<PT( ambientprobe_t )>, int_hash> _probes;
        SimpleHashMap<int, PT( KDTree ), int_hash> _probe_kdtrees;
        pvector<ambientprobe_t *> _all_probes;
        pvector<PT( light_t )> _all_lights;
        pvector<PT( cubemap_t )> _cubemaps;
        pvector<pvector<light_t *>> _light_pvs;
        light_t *_sunlight;

       // PT( KDTree ) _light_kdtree;
        PT( KDTree ) _envmap_kdtree;

        NodePath _vis_root;

        UpdateSeq _node_sequence;

        double _last_garbage_collect_time;

        Mutex _cache_mutex;

public:
        friend class NodeWeakCallback;
};

#endif // AMBIENT_PROBES_H
