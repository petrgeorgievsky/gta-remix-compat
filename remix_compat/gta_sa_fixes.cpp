
module;
#include <d3d9.h>
module gta_remix_fixes;
import executable_injection;

[[maybe_unused]] auto d3dDevice = *(IDirect3DDevice9 **)0xC97C28;
// An empty func to no-op some calls
[[maybe_unused]] void empty_void() {}
[[maybe_unused]] bool true_ret_hook() { return true; }
[[maybe_unused]] bool false_ret_hook() { return false; }

#pragma region Lighting Fixes

/// Method to resolve lighting for object, we disable it because game creates
/// directional lights instead of point/spot/etc. lights
double CPointLights_GenerateLightsAffectingObject(
    [[maybe_unused]] D3DVECTOR *point,
    [[maybe_unused]] float *total_lighting,
    [[maybe_unused]]   void* entity)
{
  return 1.0;
}

/*
 * TODO: Implement light injection, to do that it seems that we would need
 * to hold a separate light counter
void __cdecl CPointLights_AddLight(
    char type,
    D3DVECTOR pos,
    D3DVECTOR dir,
    float radius,
    float red,
    float green,
    float blue,
    char fogType,
    char generateExtraShadows,
    void *entityAffected)
{
    static DWORD light_id = 0;
    auto dev = *(IDirect3DDevice9 **)0xC97C28;
    D3DLIGHT9 l{};
    l.Position.x = pos.x;
    l.Position.y = pos.y;
    l.Position.z = pos.z;
    l.Type = D3DLIGHTTYPE::D3DLIGHT_POINT;
    dev->SetLight(light_id, )
}*/

void fix_Lighting()
{
  RedirectJump(0x6FFBB0, &CPointLights_GenerateLightsAffectingObject);
  // Lights
  //RedirectJump( 0x7000E0, reinterpret_cast<void *>( CPointLights_AddLight ) );
}
void fix_ShadowRendering()
{
  // So far we only replace some functions with no-op, so that remix won't bug out
  RedirectCall( 0x53E096, reinterpret_cast<void *>( empty_void ) ); // RwCameraEndUpdate
  RedirectCall( 0x53E0B9, reinterpret_cast<void *>( empty_void ) ); // RwCameraBeginUpdate
  RedirectCall( 0x53E0D3, reinterpret_cast<void *>( empty_void ) ); // RwCameraEndUpdate
  RedirectCall( 0x53E0EF, reinterpret_cast<void *>( empty_void ) ); // RwCameraBeginUpdate
  // Static shadows
  // TODO: Some of those aren't shadows but decals, so it'd be better to filter only shadows and ignore decals
  RedirectCall( 0x53E0BE, reinterpret_cast<void *>( empty_void ) ); // UpdateStaticShadows
  RedirectCall( 0x53E0C3, reinterpret_cast<void *>( empty_void ) ); // RenderStaticShadows
  RedirectCall( 0x53E0C8, reinterpret_cast<void *>( empty_void ) ); // RenderStoredShadows
  // Dynamic shadows
  RedirectCall( 0x53EA0D,
               reinterpret_cast<void *>(
                   empty_void ) ); // RealTimeShadowManager_Update
}
#pragma endregion Lighting Fixes

void fix_LodRendering()
{
  RedirectCall( 0x553C78, reinterpret_cast<void *>( empty_void ) ); // RwCameraEndUpdate
  RedirectCall( 0x553C9A, reinterpret_cast<void *>( empty_void ) ); // RwCameraBeginUpdate
  RedirectCall( 0x553CD1, reinterpret_cast<void *>( empty_void ) ); // RwCameraEndUpdate
  RedirectCall( 0x553CEC, reinterpret_cast<void *>( empty_void ) ); // RwCameraBeginUpdate
}

void fix_MirrorRendering()
{
  // RedirectCall( 0x53EA12,
  //              reinterpret_cast<void *>(
  //                  prepare_time_cycle ) ); // Mirrors_BeforeMainRender
  RedirectCall( 0x53EABA,
               reinterpret_cast<void *>(
                   empty_void ) ); // Mirrors_RenderMirrorBuffer
}

void fix_PostFXRendering()
{
  //RedirectCall(
  //    0x748A30,
  //    reinterpret_cast<void *>( empty_void ) ); // GammaInitialise
  // RedirectCall( 0x5BD779,
  //              reinterpret_cast<void *>(
  //                  empty_void ) ); // PostEffects__Initialise
  RedirectCall( 0x53E227, reinterpret_cast<void *>( true_ret_hook ) ); // PostEffects__Render
}

void GTA_RemixBugFix_Inject()
{
  fix_Lighting();
  fix_LodRendering();
  fix_ShadowRendering();
  fix_MirrorRendering();
  fix_PostFXRendering();

  //RedirectCall(
  //    0x53EAC4,
  //    reinterpret_cast<void *>(
  //         empty_void ) ); // CVisibilityPlugins21RenderWeaponPedsForPC
  // Clouds/Sky
  //RedirectJump( 0x714650,
  //              reinterpret_cast<void *>( RenderSkyPolys ) );
  // Enable Z-Test for clouds
  //uint8_t ztest = 1;
  //Patch( 0x71397E, &ztest );
}