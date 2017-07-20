//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2016 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "DX11Texture.h"
#include "DX11Renderer.h"
#include "..\IRenderer.h"

#include <d2d1.h>
#include <wincodec.h>
#include <memory>

SP_NMSPACE_BEG


const char* GetWICPixelFormatName(WICPixelFormatGUID fmt)
{
	if (fmt == GUID_WICPixelFormatDontCare) return "GUID_WICPixelFormatDontCare";
	else if (fmt == GUID_WICPixelFormatUndefined) return "GUID_WICPixelFormatUndefined";
	else if (fmt == GUID_WICPixelFormat1bppIndexed) return "GUID_WICPixelFormat1bppIndexed";
	else if (fmt == GUID_WICPixelFormat2bppIndexed) return "GUID_WICPixelFormat2bppIndexed";
	else if (fmt == GUID_WICPixelFormat4bppIndexed) return "GUID_WICPixelFormat4bppIndexed";
	else if (fmt == GUID_WICPixelFormat8bppIndexed) return "GUID_WICPixelFormat8bppIndexed";
	else if (fmt == GUID_WICPixelFormatBlackWhite) return "GUID_WICPixelFormatBlackWhite";
	else if (fmt == GUID_WICPixelFormat2bppGray) return "GUID_WICPixelFormat2bppGray";
	else if (fmt == GUID_WICPixelFormat4bppGray) return "GUID_WICPixelFormat4bppGray";
	else if (fmt == GUID_WICPixelFormat8bppGray) return "GUID_WICPixelFormat8bppGray";
	else if (fmt == GUID_WICPixelFormat8bppAlpha) return "GUID_WICPixelFormat8bppAlpha";
	else if (fmt == GUID_WICPixelFormat16bppBGR555) return "GUID_WICPixelFormat16bppBGR555";
	else if (fmt == GUID_WICPixelFormat16bppBGR565) return "GUID_WICPixelFormat16bppBGR565";
	else if (fmt == GUID_WICPixelFormat16bppBGRA5551) return "GUID_WICPixelFormat16bppBGRA5551";
	else if (fmt == GUID_WICPixelFormat16bppGray) return "GUID_WICPixelFormat16bppGray";
	else if (fmt == GUID_WICPixelFormat24bppBGR) return "GUID_WICPixelFormat24bppBGR";
	else if (fmt == GUID_WICPixelFormat24bppRGB) return "GUID_WICPixelFormat24bppRGB";
	else if (fmt == GUID_WICPixelFormat32bppBGR) return "GUID_WICPixelFormat32bppBGR";
	else if (fmt == GUID_WICPixelFormat32bppBGRA) return "GUID_WICPixelFormat32bppBGRA";
	else if (fmt == GUID_WICPixelFormat32bppPBGRA) return "GUID_WICPixelFormat32bppPBGRA";
	else if (fmt == GUID_WICPixelFormat32bppGrayFloat) return "GUID_WICPixelFormat32bppGrayFloat";
	else if (fmt == GUID_WICPixelFormat32bppRGB) return "GUID_WICPixelFormat32bppRGB";
	else if (fmt == GUID_WICPixelFormat32bppRGBA) return "GUID_WICPixelFormat32bppRGBA";
	else if (fmt == GUID_WICPixelFormat32bppPRGBA) return "GUID_WICPixelFormat32bppPRGBA";
	else if (fmt == GUID_WICPixelFormat48bppRGB) return "GUID_WICPixelFormat48bppRGB";
	else if (fmt == GUID_WICPixelFormat48bppBGR) return "GUID_WICPixelFormat48bppBGR";
	else if (fmt == GUID_WICPixelFormat64bppRGB) return "GUID_WICPixelFormat64bppRGB";
	else if (fmt == GUID_WICPixelFormat64bppRGBA) return "GUID_WICPixelFormat64bppRGBA";
	else if (fmt == GUID_WICPixelFormat64bppBGRA) return "GUID_WICPixelFormat64bppBGRA";
	else if (fmt == GUID_WICPixelFormat64bppPRGBA) return "GUID_WICPixelFormat64bppPRGBA";
	else if (fmt == GUID_WICPixelFormat64bppPBGRA) return "GUID_WICPixelFormat64bppPBGRA";
	else if (fmt == GUID_WICPixelFormat16bppGrayFixedPoint) return "GUID_WICPixelFormat16bppGrayFixedPoint";
	else if (fmt == GUID_WICPixelFormat32bppBGR101010) return "GUID_WICPixelFormat32bppBGR101010";
	else if (fmt == GUID_WICPixelFormat48bppRGBFixedPoint) return "GUID_WICPixelFormat48bppRGBFixedPoint";
	else if (fmt == GUID_WICPixelFormat48bppBGRFixedPoint) return "GUID_WICPixelFormat48bppBGRFixedPoint";
	else if (fmt == GUID_WICPixelFormat96bppRGBFixedPoint) return "GUID_WICPixelFormat96bppRGBFixedPoint";
	else if (fmt == GUID_WICPixelFormat96bppRGBFloat) return "GUID_WICPixelFormat96bppRGBFloat";
	else if (fmt == GUID_WICPixelFormat128bppRGBAFloat) return "GUID_WICPixelFormat128bppRGBAFloat";
	else if (fmt == GUID_WICPixelFormat128bppPRGBAFloat) return "GUID_WICPixelFormat128bppPRGBAFloat";
	else if (fmt == GUID_WICPixelFormat128bppRGBFloat) return "GUID_WICPixelFormat128bppRGBFloat";
	else if (fmt == GUID_WICPixelFormat32bppCMYK) return "GUID_WICPixelFormat32bppCMYK";
	else if (fmt == GUID_WICPixelFormat64bppRGBAFixedPoint) return "GUID_WICPixelFormat64bppRGBAFixedPoint";
	else if (fmt == GUID_WICPixelFormat64bppBGRAFixedPoint) return "GUID_WICPixelFormat64bppBGRAFixedPoint";
	else if (fmt == GUID_WICPixelFormat64bppRGBFixedPoint) return "GUID_WICPixelFormat64bppRGBFixedPoint";
	else if (fmt == GUID_WICPixelFormat128bppRGBAFixedPoint) return "GUID_WICPixelFormat128bppRGBAFixedPoint";
	else if (fmt == GUID_WICPixelFormat128bppRGBFixedPoint) return "GUID_WICPixelFormat128bppRGBFixedPoint";
	else if (fmt == GUID_WICPixelFormat64bppRGBAHalf) return "GUID_WICPixelFormat64bppRGBAHalf";
	else if (fmt == GUID_WICPixelFormat64bppRGBHalf) return "GUID_WICPixelFormat64bppRGBHalf";
	else if (fmt == GUID_WICPixelFormat48bppRGBHalf) return "GUID_WICPixelFormat48bppRGBHalf";
	else if (fmt == GUID_WICPixelFormat32bppRGBE) return "GUID_WICPixelFormat32bppRGBE";
	else if (fmt == GUID_WICPixelFormat16bppGrayHalf) return "GUID_WICPixelFormat16bppGrayHalf";
	else if (fmt == GUID_WICPixelFormat32bppGrayFixedPoint) return "GUID_WICPixelFormat32bppGrayFixedPoint";
	else if (fmt == GUID_WICPixelFormat32bppRGBA1010102) return "GUID_WICPixelFormat32bppRGBA1010102";
	else if (fmt == GUID_WICPixelFormat32bppRGBA1010102XR) return "GUID_WICPixelFormat32bppRGBA1010102XR";
	else if (fmt == GUID_WICPixelFormat64bppCMYK) return "GUID_WICPixelFormat64bppCMYK";
	else if (fmt == GUID_WICPixelFormat24bpp3Channels) return "GUID_WICPixelFormat24bpp3Channels";
	else if (fmt == GUID_WICPixelFormat32bpp4Channels) return "GUID_WICPixelFormat32bpp4Channels";
	else if (fmt == GUID_WICPixelFormat40bpp5Channels) return "GUID_WICPixelFormat40bpp5Channels";
	else if (fmt == GUID_WICPixelFormat48bpp6Channels) return "GUID_WICPixelFormat48bpp6Channels";
	else if (fmt == GUID_WICPixelFormat56bpp7Channels) return "GUID_WICPixelFormat56bpp7Channels";
	else if (fmt == GUID_WICPixelFormat64bpp8Channels) return "GUID_WICPixelFormat64bpp8Channels";
	else if (fmt == GUID_WICPixelFormat48bpp3Channels) return "GUID_WICPixelFormat48bpp3Channels";
	else if (fmt == GUID_WICPixelFormat64bpp4Channels) return "GUID_WICPixelFormat64bpp4Channels";
	else if (fmt == GUID_WICPixelFormat80bpp5Channels) return "GUID_WICPixelFormat80bpp5Channels";
	else if (fmt == GUID_WICPixelFormat96bpp6Channels) return "GUID_WICPixelFormat96bpp6Channels";
	else if (fmt == GUID_WICPixelFormat112bpp7Channels) return "GUID_WICPixelFormat112bpp7Channels";
	else if (fmt == GUID_WICPixelFormat128bpp8Channels) return "GUID_WICPixelFormat128bpp8Channels";
	else if (fmt == GUID_WICPixelFormat40bppCMYKAlpha) return "GUID_WICPixelFormat40bppCMYKAlpha";
	else if (fmt == GUID_WICPixelFormat80bppCMYKAlpha) return "GUID_WICPixelFormat80bppCMYKAlpha";
	else if (fmt == GUID_WICPixelFormat32bpp3ChannelsAlpha) return "GUID_WICPixelFormat32bpp3ChannelsAlpha";
	else if (fmt == GUID_WICPixelFormat40bpp4ChannelsAlpha) return "GUID_WICPixelFormat40bpp4ChannelsAlpha";
	else if (fmt == GUID_WICPixelFormat48bpp5ChannelsAlpha) return "GUID_WICPixelFormat48bpp5ChannelsAlpha";
	else if (fmt == GUID_WICPixelFormat56bpp6ChannelsAlpha) return "GUID_WICPixelFormat56bpp6ChannelsAlpha";
	else if (fmt == GUID_WICPixelFormat64bpp7ChannelsAlpha) return "GUID_WICPixelFormat64bpp7ChannelsAlpha";
	else if (fmt == GUID_WICPixelFormat72bpp8ChannelsAlpha) return "GUID_WICPixelFormat72bpp8ChannelsAlpha";
	else if (fmt == GUID_WICPixelFormat64bpp3ChannelsAlpha) return "GUID_WICPixelFormat64bpp3ChannelsAlpha";
	else if (fmt == GUID_WICPixelFormat80bpp4ChannelsAlpha) return "GUID_WICPixelFormat80bpp4ChannelsAlpha";
	else if (fmt == GUID_WICPixelFormat96bpp5ChannelsAlpha) return "GUID_WICPixelFormat96bpp5ChannelsAlpha";
	else if (fmt == GUID_WICPixelFormat112bpp6ChannelsAlpha) return "GUID_WICPixelFormat112bpp6ChannelsAlpha";
	else if (fmt == GUID_WICPixelFormat128bpp7ChannelsAlpha) return "GUID_WICPixelFormat128bpp7ChannelsAlpha";
	else if (fmt == GUID_WICPixelFormat144bpp8ChannelsAlpha) return "GUID_WICPixelFormat144bpp8ChannelsAlpha";
	else if (fmt == GUID_WICPixelFormat8bppY) return "GUID_WICPixelFormat8bppY";
	else if (fmt == GUID_WICPixelFormat8bppCb) return "GUID_WICPixelFormat8bppCb";
	else if (fmt == GUID_WICPixelFormat8bppCr) return "GUID_WICPixelFormat8bppCr";
	else if (fmt == GUID_WICPixelFormat16bppCbCr) return "GUID_WICPixelFormat16bppCbCr";
	else if (fmt == GUID_WICPixelFormat16bppYQuantizedDctCoefficients) return "GUID_WICPixelFormat16bppYQuantizedDctCoefficients";
	else if (fmt == GUID_WICPixelFormat16bppCbQuantizedDctCoefficients) return "GUID_WICPixelFormat16bppCbQuantizedDctCoefficients";
	else if (fmt == GUID_WICPixelFormat16bppCrQuantizedDctCoefficients) return "GUID_WICPixelFormat16bppCrQuantizedDctCoefficients";
	else return "Unknown format";
}

const char* GetDXGIFormatName(DXGI_FORMAT format)
{
	switch (format)
	{
	case DXGI_FORMAT_UNKNOWN: return "DXGI_FORMAT_UNKNOWN";
	case DXGI_FORMAT_R32G32B32A32_TYPELESS: return "DXGI_FORMAT_R32G32B32A32_TYPELESS";
	case DXGI_FORMAT_R32G32B32A32_FLOAT: return "DXGI_FORMAT_R32G32B32A32_FLOAT";
	case DXGI_FORMAT_R32G32B32A32_UINT: return "DXGI_FORMAT_R32G32B32A32_UINT";
	case DXGI_FORMAT_R32G32B32A32_SINT: return "DXGI_FORMAT_R32G32B32A32_SINT";
	case DXGI_FORMAT_R32G32B32_TYPELESS: return "DXGI_FORMAT_R32G32B32_TYPELESS";
	case DXGI_FORMAT_R32G32B32_FLOAT: return "DXGI_FORMAT_R32G32B32_FLOAT";
	case DXGI_FORMAT_R32G32B32_UINT: return "DXGI_FORMAT_R32G32B32_UINT";
	case DXGI_FORMAT_R32G32B32_SINT: return "DXGI_FORMAT_R32G32B32_SINT";
	case DXGI_FORMAT_R16G16B16A16_TYPELESS: return "DXGI_FORMAT_R16G16B16A16_TYPELESS";
	case DXGI_FORMAT_R16G16B16A16_FLOAT: return "DXGI_FORMAT_R16G16B16A16_FLOAT";
	case DXGI_FORMAT_R16G16B16A16_UNORM: return "DXGI_FORMAT_R16G16B16A16_UNORM";
	case DXGI_FORMAT_R16G16B16A16_UINT: return "DXGI_FORMAT_R16G16B16A16_UINT";
	case DXGI_FORMAT_R16G16B16A16_SNORM: return "DXGI_FORMAT_R16G16B16A16_SNORM";
	case DXGI_FORMAT_R16G16B16A16_SINT: return "DXGI_FORMAT_R16G16B16A16_SINT";
	case DXGI_FORMAT_R32G32_TYPELESS: return "DXGI_FORMAT_R32G32_TYPELESS";
	case DXGI_FORMAT_R32G32_FLOAT: return "DXGI_FORMAT_R32G32_FLOAT";
	case DXGI_FORMAT_R32G32_UINT: return "DXGI_FORMAT_R32G32_UINT";
	case DXGI_FORMAT_R32G32_SINT: return "DXGI_FORMAT_R32G32_SINT";
	case DXGI_FORMAT_R32G8X24_TYPELESS: return "DXGI_FORMAT_R32G8X24_TYPELESS";
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT: return "DXGI_FORMAT_D32_FLOAT_S8X24_UINT";
	case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS: return "DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS";
	case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT: return "DXGI_FORMAT_X32_TYPELESS_G8X24_UINT";
	case DXGI_FORMAT_R10G10B10A2_TYPELESS: return "DXGI_FORMAT_R10G10B10A2_TYPELESS";
	case DXGI_FORMAT_R10G10B10A2_UNORM: return "DXGI_FORMAT_R10G10B10A2_UNORM";
	case DXGI_FORMAT_R10G10B10A2_UINT: return "DXGI_FORMAT_R10G10B10A2_UINT";
	case DXGI_FORMAT_R11G11B10_FLOAT: return "DXGI_FORMAT_R11G11B10_FLOAT";
	case DXGI_FORMAT_R8G8B8A8_TYPELESS: return "DXGI_FORMAT_R8G8B8A8_TYPELESS";
	case DXGI_FORMAT_R8G8B8A8_UNORM: return "DXGI_FORMAT_R8G8B8A8_UNORM";
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB: return "DXGI_FORMAT_R8G8B8A8_UNORM_SRGB";
	case DXGI_FORMAT_R8G8B8A8_UINT: return "DXGI_FORMAT_R8G8B8A8_UINT";
	case DXGI_FORMAT_R8G8B8A8_SNORM: return "DXGI_FORMAT_R8G8B8A8_SNORM";
	case DXGI_FORMAT_R8G8B8A8_SINT: return "DXGI_FORMAT_R8G8B8A8_SINT";
	case DXGI_FORMAT_R16G16_TYPELESS: return "DXGI_FORMAT_R16G16_TYPELESS";
	case DXGI_FORMAT_R16G16_FLOAT: return "DXGI_FORMAT_R16G16_FLOAT";
	case DXGI_FORMAT_R16G16_UNORM: return "DXGI_FORMAT_R16G16_UNORM";
	case DXGI_FORMAT_R16G16_UINT: return "DXGI_FORMAT_R16G16_UINT";
	case DXGI_FORMAT_R16G16_SNORM: return "DXGI_FORMAT_R16G16_SNORM";
	case DXGI_FORMAT_R16G16_SINT: return "DXGI_FORMAT_R16G16_SINT";
	case DXGI_FORMAT_R32_TYPELESS: return "DXGI_FORMAT_R32_TYPELESS";
	case DXGI_FORMAT_D32_FLOAT: return "DXGI_FORMAT_D32_FLOAT";
	case DXGI_FORMAT_R32_FLOAT: return "DXGI_FORMAT_R32_FLOAT";
	case DXGI_FORMAT_R32_UINT: return "DXGI_FORMAT_R32_UINT";
	case DXGI_FORMAT_R32_SINT: return "DXGI_FORMAT_R32_SINT";
	case DXGI_FORMAT_R24G8_TYPELESS: return "DXGI_FORMAT_R24G8_TYPELESS";
	case DXGI_FORMAT_D24_UNORM_S8_UINT: return "DXGI_FORMAT_D24_UNORM_S8_UINT";
	case DXGI_FORMAT_R24_UNORM_X8_TYPELESS: return "DXGI_FORMAT_R24_UNORM_X8_TYPELESS";
	case DXGI_FORMAT_X24_TYPELESS_G8_UINT: return "DXGI_FORMAT_X24_TYPELESS_G8_UINT";
	case DXGI_FORMAT_R8G8_TYPELESS: return "DXGI_FORMAT_R8G8_TYPELESS";
	case DXGI_FORMAT_R8G8_UNORM: return "DXGI_FORMAT_R8G8_UNORM";
	case DXGI_FORMAT_R8G8_UINT: return "DXGI_FORMAT_R8G8_UINT";
	case DXGI_FORMAT_R8G8_SNORM: return "DXGI_FORMAT_R8G8_SNORM";
	case DXGI_FORMAT_R8G8_SINT: return "DXGI_FORMAT_R8G8_SINT";
	case DXGI_FORMAT_R16_TYPELESS: return "DXGI_FORMAT_R16_TYPELESS";
	case DXGI_FORMAT_R16_FLOAT: return "DXGI_FORMAT_R16_FLOAT";
	case DXGI_FORMAT_D16_UNORM: return "DXGI_FORMAT_D16_UNORM";
	case DXGI_FORMAT_R16_UNORM: return "DXGI_FORMAT_R16_UNORM";
	case DXGI_FORMAT_R16_UINT: return "DXGI_FORMAT_R16_UINT";
	case DXGI_FORMAT_R16_SNORM: return "DXGI_FORMAT_R16_SNORM";
	case DXGI_FORMAT_R16_SINT: return "DXGI_FORMAT_R16_SINT";
	case DXGI_FORMAT_R8_TYPELESS: return "DXGI_FORMAT_R8_TYPELESS";
	case DXGI_FORMAT_R8_UNORM: return "DXGI_FORMAT_R8_UNORM";
	case DXGI_FORMAT_R8_UINT: return "DXGI_FORMAT_R8_UINT";
	case DXGI_FORMAT_R8_SNORM: return "DXGI_FORMAT_R8_SNORM";
	case DXGI_FORMAT_R8_SINT: return "DXGI_FORMAT_R8_SINT";
	case DXGI_FORMAT_A8_UNORM: return "DXGI_FORMAT_A8_UNORM";
	case DXGI_FORMAT_R1_UNORM: return "DXGI_FORMAT_R1_UNORM";
	case DXGI_FORMAT_R9G9B9E5_SHAREDEXP: return "DXGI_FORMAT_R9G9B9E5_SHAREDEXP";
	case DXGI_FORMAT_R8G8_B8G8_UNORM: return "DXGI_FORMAT_R8G8_B8G8_UNORM";
	case DXGI_FORMAT_G8R8_G8B8_UNORM: return "DXGI_FORMAT_G8R8_G8B8_UNORM";
	case DXGI_FORMAT_BC1_TYPELESS: return "DXGI_FORMAT_BC1_TYPELESS";
	case DXGI_FORMAT_BC1_UNORM: return "DXGI_FORMAT_BC1_UNORM";
	case DXGI_FORMAT_BC1_UNORM_SRGB: return "DXGI_FORMAT_BC1_UNORM_SRGB";
	case DXGI_FORMAT_BC2_TYPELESS: return "DXGI_FORMAT_BC2_TYPELESS";
	case DXGI_FORMAT_BC2_UNORM: return "DXGI_FORMAT_BC2_UNORM";
	case DXGI_FORMAT_BC2_UNORM_SRGB: return "DXGI_FORMAT_BC2_UNORM_SRGB";
	case DXGI_FORMAT_BC3_TYPELESS: return "DXGI_FORMAT_BC3_TYPELESS";
	case DXGI_FORMAT_BC3_UNORM: return "DXGI_FORMAT_BC3_UNORM";
	case DXGI_FORMAT_BC3_UNORM_SRGB: return "DXGI_FORMAT_BC3_UNORM_SRGB";
	case DXGI_FORMAT_BC4_TYPELESS: return "DXGI_FORMAT_BC4_TYPELESS";
	case DXGI_FORMAT_BC4_UNORM: return "DXGI_FORMAT_BC4_UNORM";
	case DXGI_FORMAT_BC4_SNORM: return "DXGI_FORMAT_BC4_SNORM";
	case DXGI_FORMAT_BC5_TYPELESS: return "DXGI_FORMAT_BC5_TYPELESS";
	case DXGI_FORMAT_BC5_UNORM: return "DXGI_FORMAT_BC5_UNORM";
	case DXGI_FORMAT_BC5_SNORM: return "DXGI_FORMAT_BC5_SNORM";
	case DXGI_FORMAT_B5G6R5_UNORM: return "DXGI_FORMAT_B5G6R5_UNORM";
	case DXGI_FORMAT_B5G5R5A1_UNORM: return "DXGI_FORMAT_B5G5R5A1_UNORM";
	case DXGI_FORMAT_B8G8R8A8_UNORM: return "DXGI_FORMAT_B8G8R8A8_UNORM";
	case DXGI_FORMAT_B8G8R8X8_UNORM: return "DXGI_FORMAT_B8G8R8X8_UNORM";
	case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM: return "DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM";
	case DXGI_FORMAT_B8G8R8A8_TYPELESS: return "DXGI_FORMAT_B8G8R8A8_TYPELESS";
	case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB: return "DXGI_FORMAT_B8G8R8A8_UNORM_SRGB";
	case DXGI_FORMAT_B8G8R8X8_TYPELESS: return "DXGI_FORMAT_B8G8R8X8_TYPELESS";
	case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB: return "DXGI_FORMAT_B8G8R8X8_UNORM_SRGB";
	case DXGI_FORMAT_BC6H_TYPELESS: return "DXGI_FORMAT_BC6H_TYPELESS";
	case DXGI_FORMAT_BC6H_UF16: return "DXGI_FORMAT_BC6H_UF16";
	case DXGI_FORMAT_BC6H_SF16: return "DXGI_FORMAT_BC6H_SF16";
	case DXGI_FORMAT_BC7_TYPELESS: return "DXGI_FORMAT_BC7_TYPELESS";
	case DXGI_FORMAT_BC7_UNORM: return "DXGI_FORMAT_BC7_UNORM";
	case DXGI_FORMAT_BC7_UNORM_SRGB: return "DXGI_FORMAT_BC7_UNORM_SRGB";
	case DXGI_FORMAT_AYUV: return "DXGI_FORMAT_AYUV";
	case DXGI_FORMAT_Y410: return "DXGI_FORMAT_Y410";
	case DXGI_FORMAT_Y416: return "DXGI_FORMAT_Y416";
	case DXGI_FORMAT_NV12: return "DXGI_FORMAT_NV12";
	case DXGI_FORMAT_P010: return "DXGI_FORMAT_P010";
	case DXGI_FORMAT_P016: return "DXGI_FORMAT_P016";
	case DXGI_FORMAT_420_OPAQUE: return "DXGI_FORMAT_420_OPAQUE";
	case DXGI_FORMAT_YUY2: return "DXGI_FORMAT_YUY2";
	case DXGI_FORMAT_Y210: return "DXGI_FORMAT_Y210";
	case DXGI_FORMAT_Y216: return "DXGI_FORMAT_Y216";
	case DXGI_FORMAT_NV11: return "DXGI_FORMAT_NV11";
	case DXGI_FORMAT_AI44: return "DXGI_FORMAT_AI44";
	case DXGI_FORMAT_IA44: return "DXGI_FORMAT_IA44";
	case DXGI_FORMAT_P8: return "DXGI_FORMAT_P8";
	case DXGI_FORMAT_A8P8: return "DXGI_FORMAT_A8P8";
	case DXGI_FORMAT_B4G4R4A4_UNORM: return "DXGI_FORMAT_B4G4R4A4_UNORM";
	case DXGI_FORMAT_P208: return "DXGI_FORMAT_P208";
	case DXGI_FORMAT_V208: return "DXGI_FORMAT_V208";
	case DXGI_FORMAT_V408: return "DXGI_FORMAT_V408";
	default: return "???";
	}
}



#define TEXTURE_FORMAT_MAPPING(format1, format2) \
	if (fmt1Tofmt2 & (fmt1 == format1)) { fmt2 = format2; return; } \
	else if (fmt2 == format2) { fmt1 = format1; return; }


void MapWICFormatDXGIFormat(WICPixelFormatGUID& fmt1, DXGI_FORMAT& fmt2, bool fmt1Tofmt2 = true)
{
	
	TEXTURE_FORMAT_MAPPING(GUID_WICPixelFormat128bppRGBAFloat, DXGI_FORMAT_R32G32B32A32_FLOAT);
	TEXTURE_FORMAT_MAPPING(GUID_WICPixelFormat64bppRGBAHalf, DXGI_FORMAT_R16G16B16A16_FLOAT);
	TEXTURE_FORMAT_MAPPING(GUID_WICPixelFormat64bppRGBA, DXGI_FORMAT_R16G16B16A16_UNORM);
	TEXTURE_FORMAT_MAPPING(GUID_WICPixelFormat32bppRGBA, DXGI_FORMAT_R8G8B8A8_UNORM);
	TEXTURE_FORMAT_MAPPING(GUID_WICPixelFormat32bppBGRA, DXGI_FORMAT_B8G8R8A8_UNORM);
	TEXTURE_FORMAT_MAPPING(GUID_WICPixelFormat32bppBGR, DXGI_FORMAT_B8G8R8X8_UNORM);
	TEXTURE_FORMAT_MAPPING(GUID_WICPixelFormat32bppRGBA1010102XR, DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM);
	TEXTURE_FORMAT_MAPPING(GUID_WICPixelFormat32bppRGBA1010102, DXGI_FORMAT_R10G10B10A2_UNORM);
	TEXTURE_FORMAT_MAPPING(GUID_WICPixelFormat32bppRGBE, DXGI_FORMAT_R9G9B9E5_SHAREDEXP);
	TEXTURE_FORMAT_MAPPING(GUID_WICPixelFormat16bppBGRA5551, DXGI_FORMAT_B5G5R5A1_UNORM);
	TEXTURE_FORMAT_MAPPING(GUID_WICPixelFormat16bppBGR565, DXGI_FORMAT_B5G6R5_UNORM);
	TEXTURE_FORMAT_MAPPING(GUID_WICPixelFormat32bppGrayFloat, DXGI_FORMAT_R32_FLOAT);
	TEXTURE_FORMAT_MAPPING(GUID_WICPixelFormat16bppGrayHalf, DXGI_FORMAT_R16_FLOAT);
	TEXTURE_FORMAT_MAPPING(GUID_WICPixelFormat16bppGray, DXGI_FORMAT_R16_UNORM);
	TEXTURE_FORMAT_MAPPING(GUID_WICPixelFormat8bppGray, DXGI_FORMAT_R8_UNORM);
	TEXTURE_FORMAT_MAPPING(GUID_WICPixelFormat8bppAlpha, DXGI_FORMAT_A8_UNORM);
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8)
	TEXTURE_FORMAT_MAPPING(GUID_WICPixelFormat96bppRGBFloat, DXGI_FORMAT_R32G32B32_FLOAT);
#endif
	
	if (fmt1Tofmt2)
	{
		CLog::Log(S_WARNING, "Could not map WIC pixel format %s to DXGI format", GetWICPixelFormatName(fmt1));
		fmt2 = DXGI_FORMAT_UNKNOWN;
	}
	else
	{
		CLog::Log(S_WARNING, "Could not map DXGI format %s to WIC pixel format", GetDXGIFormatName(fmt2));
		fmt1 = GUID_WICPixelFormatUndefined;
	}
}


// fmt1Tofmt2 - if true, fmt1 is converted to fmt2, otherwise fmt2 is converted to fmt1
S_API static void MapTextureType(DXGI_FORMAT& fmt1, ETextureType& fmt2, bool fmt1Tofmt2)
{
	TEXTURE_FORMAT_MAPPING(DXGI_FORMAT_R32_FLOAT, eTEXTURE_R32_FLOAT);
	TEXTURE_FORMAT_MAPPING(DXGI_FORMAT_R32_TYPELESS, eTEXTURE_R32_FLOAT);
	TEXTURE_FORMAT_MAPPING(DXGI_FORMAT_D32_FLOAT, eTEXTURE_D32_FLOAT);
	TEXTURE_FORMAT_MAPPING(DXGI_FORMAT_R16G16B16A16_FLOAT, eTEXTURE_R16G16B16A16_FLOAT);
	TEXTURE_FORMAT_MAPPING(DXGI_FORMAT_R16G16_FLOAT, eTEXTURE_R16G16_FLOAT);
	TEXTURE_FORMAT_MAPPING(DXGI_FORMAT_R8G8B8A8_UNORM, eTEXTURE_R8G8B8A8_UNORM);

	if (fmt1Tofmt2)
	{
		CLog::Log(S_WARNING, "Could not map dxgi format %d to SpeedPoint texture type", (int)fmt1);
		fmt2 = eTEXTURE_R8G8B8A8_UNORM;
	}
	else
	{
		CLog::Log(S_WARNING, "Could not map texture type %s to DXGI format", GetTextureTypeName(fmt2));
		fmt1 = DXGI_FORMAT_R8G8B8A8_UNORM;
	}
}



// -----------------------------------------------------------------------------------------------
DX11Texture::DX11Texture(DX11Renderer* pDXRenderer)
: m_pDXRenderer(pDXRenderer),
m_Specification("?notinitialized?"),
m_bDynamic(false),
m_pDXTexture(0),
m_pDXStagingTexture(0),
m_pDXSRV(nullptr),
m_pLockedData(nullptr),
m_nLockedBytes(0),
m_pStagedData(0),
m_bStaged(false),
m_bLocked(false),
m_bIsCubemap(false),
m_bArray(false)
{
}

// -----------------------------------------------------------------------------------------------
DX11Texture::~DX11Texture()
{
	if (m_RefCount > 0)
	{
		CLog::Log(S_WARNING, "Warning: Destructing texture 0x%08X ('%s') with refcount = %u",
			(unsigned long)m_pDXTexture, m_Specification.c_str(), m_RefCount);
	}

	Clear();
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Texture::LoadTextureImage(const string& cFileName, SLoadedTextureBitmap& bitmap)
{
	HRESULT hRes;

	IWICImagingFactory* pImgFactory;
	hRes = CoCreateInstance(CLSID_WICImagingFactory, 0, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (void**)&pImgFactory);
	if (Failure(hRes))
	{
		return CLog::Log(S_ERROR, "Failed create WIC Imaging Factory!");
	}

	IWICBitmapDecoder* pImgDecoder;
	wchar_t* cWFilename = new wchar_t[cFileName.length() + 1];
	size_t nNumCharsConv;
	mbstowcs_s(&nNumCharsConv, cWFilename, cFileName.length() + 1, cFileName.c_str(), _TRUNCATE);
	hRes = pImgFactory->CreateDecoderFromFilename(cWFilename, 0, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &pImgDecoder);
	if (Failure(hRes))
	{
		CLog::Log(S_DEBUG, "Failed create WIC Image decoder for %s (cWFilename=%S)!", cFileName.c_str(), cWFilename);
		return S_ERROR;
	}


	//
	// TODO:
	//	for animated texture, loop through all frames and call pImgFactory->GetFrameCount() first.
	//



	// Decode frame
	IWICBitmapFrameDecode* pBmpFrameDecode;
	pImgDecoder->GetFrame(0, &pBmpFrameDecode);


	// Get pixel format from loaded bitmap frame
	WICPixelFormatGUID srcFmtWIC;
	if (Failure(pBmpFrameDecode->GetPixelFormat(&srcFmtWIC)))
	{
		return CLog::Log(S_ERROR, "Failed get pixel format of desired frame!");
	}

	WICPixelFormatGUID dstFmtWIC;
	DXGI_FORMAT dstFmtDXGI;

	if (bitmap.format != DXGI_FORMAT_UNKNOWN)
	{
		// Desired format was specified in bitmap structure. Try to use this.
		dstFmtDXGI = bitmap.format;
		MapWICFormatDXGIFormat(dstFmtWIC, dstFmtDXGI, false);
		if (dstFmtDXGI == DXGI_FORMAT_UNKNOWN || dstFmtWIC == GUID_WICPixelFormatUndefined)
		{
			return CLog::Log(S_ERROR, "DX11Texture::LoadTextureImage(): Desired format is unsupported (%s) for file %s",
				GetDXGIFormatName(bitmap.format), cFileName.c_str());
		}
	}
	else
	{
		// Some pixel format cannot be directly translated into a DXGI Format. So find a nearest match.	
		dstFmtWIC = srcFmtWIC;
		if (srcFmtWIC == GUID_WICPixelFormat24bppBGR) dstFmtWIC = GUID_WICPixelFormat32bppRGBA;
		else if (srcFmtWIC == GUID_WICPixelFormat8bppIndexed) dstFmtWIC = GUID_WICPixelFormat32bppRGBA;
		else if (srcFmtWIC == GUID_WICPixelFormat32bppPBGRA) dstFmtWIC = GUID_WICPixelFormat32bppRGBA;

		// Convert WIC pixel format to DXGI format
		MapWICFormatDXGIFormat(dstFmtWIC, dstFmtDXGI);
		if (dstFmtDXGI == DXGI_FORMAT_UNKNOWN)
		{
			return CLog::Log(S_ERROR, "DX11Texture::LoadTextureImage(): Unsupported pixel format (%s) of file '%s'",
				GetWICPixelFormatName(srcFmtWIC), cFileName.c_str());
		}

		bitmap.format = dstFmtDXGI;
	}


	// ----------------------------------------------------------------------------------------------------------------------
	// Create temporary buffer

	UINT nLoadedWidth, nLoadedHeight;
	pBmpFrameDecode->GetSize(&nLoadedWidth, &nLoadedHeight);

	if (bitmap.width == 0 || bitmap.height == 0)
	{
		bitmap.width = nLoadedWidth;
		bitmap.height = nLoadedHeight;
	}

	UINT nBPP = BitsPerPixel(dstFmtWIC, pImgFactory);
	if (nBPP == 0)
	{
		return CLog::Log(S_ERROR, "DX11Texture::LoadTextureImage(): Could not retrieve bits per pixel for loaded texture!");
	}

	bitmap.imageStride = (bitmap.width * nBPP + 7) / 8;
	bitmap.imageSize = bitmap.imageStride * bitmap.height;
	bitmap.bytePerPixel = (unsigned int)(nBPP / 8);

	bitmap.buffer = new uint8_t[bitmap.imageSize];

	// ----------------------------------------------------------------------------------------------------------------------
	// Scale or convert the image to the desired format

	bool needConversion = false;
	IWICBitmapSource* pConversionSrc = 0;
	IWICBitmapScaler* pScaler = 0;

	if (nLoadedWidth == bitmap.width && nLoadedHeight == bitmap.height && memcmp(&dstFmtWIC, &srcFmtWIC, sizeof(GUID)) == 0)
	{
		// We can directly copy pixels without modification 
		hRes = pBmpFrameDecode->CopyPixels(0, static_cast<UINT>(bitmap.imageStride), static_cast<UINT>(bitmap.imageSize), bitmap.buffer);
		if (Failure(hRes))
			return CLog::Log(S_ERROR, "Failed to buffer texture!");

		needConversion = false;
	}
	else if (nLoadedWidth != bitmap.width || nLoadedHeight != bitmap.height)
	{
		// Create scaler
		if (Failure(pImgFactory->CreateBitmapScaler(&pScaler)))
		{
			CLog::Log(S_DEBUG, "Failed pImgFactory->CreateBitmapScalar()");
			return S_ERROR;
		}

		if (Failure(pScaler->Initialize(pBmpFrameDecode, bitmap.width, bitmap.height,
			WICBitmapInterpolationModeFant))) // Maybe change this someday??
		{
			CLog::Log(S_DEBUG, "Failed pScaler->Initialize()");
			return S_ERROR;
		}

		WICPixelFormatGUID pfScaler;
		if (Failure(pScaler->GetPixelFormat(&pfScaler)))
		{
			CLog::Log(S_DEBUG, "Failed pScaler->GetPixelFormat()");
			return S_ERROR;
		}

		if (memcmp(&dstFmtWIC, &pfScaler, sizeof(GUID)) == 0)
		{
			// Scale directly
			hRes = pScaler->CopyPixels(0, static_cast<UINT>(bitmap.imageStride), static_cast<UINT>(bitmap.imageSize), bitmap.buffer);
			if (Failure(hRes))
				return CLog::Log(S_ERROR, "Failed to buffer scaled texture while loading %s!", cFileName.c_str());
		}
		else
		{
			// Convert with the scaler output as source bitmap
			needConversion = true;
			pConversionSrc = pScaler;
		}
	}
	else
	{
		needConversion = true;
		pConversionSrc = pBmpFrameDecode;
	}
	
	
	if (needConversion)
	{
		ScopedTextureLoadingObject<IWICFormatConverter> formatConverter;
		if (Failure(pImgFactory->CreateFormatConverter(&formatConverter)))
			return CLog::Log(S_ERROR, "DX11Texture::LoadTextureImage(): Failed to create pixel format convert!");

		hRes = formatConverter->Initialize(pConversionSrc, dstFmtWIC, WICBitmapDitherTypeErrorDiffusion, 0, 0, WICBitmapPaletteTypeCustom);
		if (Failure(hRes))
			return CLog::Log(S_ERROR, "DX11Texture::LoadTextureImage(): Failed to initialize format converter (%s -> %s) for file '%s'!",
				GetWICPixelFormatName(srcFmtWIC), GetWICPixelFormatName(dstFmtWIC), cFileName.c_str());

		BOOL bCanConvert;
		hRes = formatConverter->CanConvert(srcFmtWIC, dstFmtWIC, &bCanConvert);
		if (Failure(hRes))
			return CLog::Log(S_ERROR, "DX11Texture::LoadTextureImage(): Could not determine if format %s is convertable to %s",
				GetWICPixelFormatName(srcFmtWIC), GetWICPixelFormatName(dstFmtWIC));

		if (!bCanConvert)
			return CLog::Log(S_ERROR, "DX11Texture::LoadTextureImage(): Format %s cannot be converted to %s!",
				GetWICPixelFormatName(srcFmtWIC), GetWICPixelFormatName(dstFmtWIC));

		hRes = formatConverter->CopyPixels(0, static_cast<UINT>(bitmap.imageStride), static_cast<UINT>(bitmap.imageSize), bitmap.buffer);
		if (Failure(hRes))
			return CLog::Log(S_ERROR, "DX11Texture::LoadTextureImage(): Failed to convert and copy pixels for file %s!", cFileName.c_str());
	}

	SP_SAFE_RELEASE(pScaler);
	SP_SAFE_RELEASE(pImgDecoder);
	SP_SAFE_RELEASE(pImgFactory);

	return S_SUCCESS;
}



// -----------------------------------------------------------------------------------------------
S_API void DX11Texture::GetCubemapImageName(string& name, ECubemapSide side)
{
	// Convert side to DX array slice side
	ECubemapSide dxSide = (ECubemapSide)GetDXCubemapArraySlice(side);

	switch (dxSide)
	{
	case eCUBEMAP_SIDE_NEGX: name += "_negx"; break;
	case eCUBEMAP_SIDE_NEGY: name += "_negy"; break;
	case eCUBEMAP_SIDE_NEGZ: name += "_negz"; break;
	case eCUBEMAP_SIDE_POSX: name += "_posx"; break;
	case eCUBEMAP_SIDE_POSY: name += "_posy"; break;
	case eCUBEMAP_SIDE_POSZ: name += "_posz"; break;
	default:
		CLog::Log(S_ERROR, "Invalid cubmap side: %d", (unsigned int)side);
		return;
	}

	name = name + ".bmp";
}

S_API unsigned int DX11Texture::GetDXCubemapArraySlice(ECubemapSide side)
{
	// Visit msdn at https://msdn.microsoft.com/en-us/library/windows/desktop/ff476906(v=vs.85).aspx	
	switch (side)
	{
	case eCUBEMAP_SIDE_NEGX: return 3; // -> POSX		
	case eCUBEMAP_SIDE_NEGY: return 0; // -> NEGX
	case eCUBEMAP_SIDE_NEGZ: return 4; // -> POSY
	case eCUBEMAP_SIDE_POSX: return 1; // -> NEGY
	case eCUBEMAP_SIDE_POSY: return 5; // -> POSZ
	case eCUBEMAP_SIDE_POSZ: return 2; // -> NEGZ
	default:
		return 0;
	}
}





// -----------------------------------------------------------------------------------------------
S_API SResult DX11Texture::LoadCubemapFromFile(const string& baseName, unsigned int singleW /*=0*/, unsigned int singleH /*=0*/)
{
	Clear();

	if (!IS_VALID_PTR(m_pDXRenderer))
		return CLog::Log(S_ERROR, "DX11Texture::LoadCubemapFromFile(): Renderer not initialized");

	SResult res;
	HRESULT hRes;

	// ----------------------------------------------------------------------------------------------------------------------
	DXGI_FORMAT loadedTextureFmt;
	std::vector<SLoadedCubemapSide> cmSides;	
		
	for (unsigned int i = 0; i < 6; ++i)
	{
		string filename = baseName;
		GetCubemapImageName(filename, (ECubemapSide)i);

		CLog::Log(S_DEBUG, "Cubmap Side Filename: %s", filename.c_str());
		
		SLoadedTextureBitmap sideBitmap;
		memset(&sideBitmap, 0, sizeof(sideBitmap));
		sideBitmap.width = singleW;
		sideBitmap.height = singleH;

		res = LoadTextureImage(filename, sideBitmap);
		if (Failure(res))
		{
			for (auto itSide = cmSides.begin(); itSide != cmSides.end(); itSide++)
				delete[] itSide->pBuffer;

			return CLog::Log(S_ERROR, "Failed load cubemap image #%d", i);
		}

		SLoadedCubemapSide side;
		side.imageSize = sideBitmap.imageSize;
		side.imageStride = sideBitmap.imageStride;
		side.pBuffer = sideBitmap.buffer;

		if (i == 0)
		{
			loadedTextureFmt = sideBitmap.format;
			singleW = sideBitmap.width;
			singleH = sideBitmap.height;
		}
		else if (sideBitmap.format != loadedTextureFmt)
		{
			for (auto itSide = cmSides.begin(); itSide != cmSides.end(); itSide++)
				delete[] itSide->pBuffer;

			return CLog::Log(S_ERROR, "Cubemap image side #%d format mismatch (expected %d, got %d).", i, (unsigned int)loadedTextureFmt, (unsigned int)sideBitmap.format);
		}

		cmSides.push_back(side);
	}	


	// ----------------------------------------------------------------------------------------------------------------------
	// Check if autogeneration of mip levels is supported

	UINT fmtSupport = 0;
	hRes = m_pDXRenderer->GetD3D11Device()->CheckFormatSupport(loadedTextureFmt, &fmtSupport);
	bool bMipAutoGenSupported = Success(hRes) && (fmtSupport & D3D11_FORMAT_SUPPORT_MIP_AUTOGEN);



	// ----------------------------------------------------------------------------------------------------------------------
	// Now create the directx texture

	unsigned int mipLevels = 2;

	D3D11_TEXTURE2D_DESC& textureDesc = m_DXTextureDesc;
	textureDesc.ArraySize = 6;
	textureDesc.BindFlags = (bMipAutoGenSupported) ? (D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET) : (D3D11_BIND_SHADER_RESOURCE);
	textureDesc.CPUAccessFlags = (m_bDynamic) ? D3D11_CPU_ACCESS_WRITE /* | D3D11_CPU_ACCESS_READ */ : 0;
	textureDesc.Format = loadedTextureFmt;
	textureDesc.Width = singleW;
	textureDesc.Height = singleH;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0; // No MS for now!

	textureDesc.MiscFlags = (bMipAutoGenSupported) ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;
	textureDesc.MiscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;

	textureDesc.BindFlags = (bMipAutoGenSupported) ? (D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET) : (D3D11_BIND_SHADER_RESOURCE);
	textureDesc.CPUAccessFlags = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.MipLevels = (bMipAutoGenSupported) ? mipLevels : 1;

	D3D11_SUBRESOURCE_DATA initData[6];

	for (unsigned int i = 0; i < 6; ++i)
	{
		initData[i].pSysMem = (void*)cmSides[i].pBuffer;
		initData[i].SysMemPitch = static_cast<UINT>(cmSides[i].imageStride);
		initData[i].SysMemSlicePitch = static_cast<UINT>(cmSides[i].imageSize);
	}

	m_pDXTexture = nullptr;

	// Create the texture
	hRes = m_pDXRenderer->GetD3D11Device()->CreateTexture2D(&textureDesc, (bMipAutoGenSupported) ? nullptr : &initData[0], &m_pDXTexture);

	if (Failure(hRes) || m_pDXTexture == nullptr)
	{
		return CLog::Log(S_ERROR, "Failed to create DirectX11 Texture!");
	}


	// ----------------------------------------------------------------------------------------------------------------------
	// Create the Shader Resource View

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	memset(&srvDesc, 0, sizeof(srvDesc));
	srvDesc.Format = loadedTextureFmt;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	//srvDesc.Texture2D.MipLevels = (bMipAutoGenSupported) ? -1 : 1;
	srvDesc.TextureCube.MipLevels = (bMipAutoGenSupported) ? mipLevels : 1;
	srvDesc.TextureCube.MostDetailedMip = 0;

	hRes = m_pDXRenderer->GetD3D11Device()->CreateShaderResourceView(m_pDXTexture, &srvDesc, &m_pDXSRV);

	if (Failure(hRes))
	{
		m_pDXTexture->Release();
		return CLog::Log(S_ERROR, "Failed create shader resource view for texture!");
	}

	if (bMipAutoGenSupported)
	{
		ID3D11DeviceContext* pDXDevCon = m_pDXRenderer->GetD3D11DeviceContext();
		for (unsigned int i = 0; i < 6; ++i)
		{
			unsigned int subresource = srvDesc.TextureCube.MipLevels * i;			
			pDXDevCon->UpdateSubresource(m_pDXTexture, subresource, nullptr, cmSides[i].pBuffer, static_cast<UINT>(cmSides[i].imageStride), static_cast<UINT>(cmSides[i].imageSize));
		}

		pDXDevCon->GenerateMips(m_pDXSRV);
	}

	// ----------------------------------------------------------------------------------------------------------------------
	// Determine type

	switch (loadedTextureFmt)
	{
	case DXGI_FORMAT_R32_FLOAT: m_Type = eTEXTURE_R32_FLOAT; break;
	case DXGI_FORMAT_D32_FLOAT: m_Type = eTEXTURE_D32_FLOAT; break;
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	default:
		m_Type = eTEXTURE_R8G8B8A8_UNORM; break;
	}

	for (auto itSide = cmSides.begin(); itSide != cmSides.end(); itSide++)
		delete[] itSide->pBuffer;

	m_bIsCubemap = true;
	return S_SUCCESS;



	// D3D11_SRV_DIMENSION_TEXTURECUBE

	/*
	D3D11_TEXTURE2D_DESC texDesc;
texDesc.Width = description.width;
texDesc.Height = description.height;
texDesc.MipLevels = 1;
texDesc.ArraySize = 6;
texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
texDesc.CPUAccessFlags = 0;
texDesc.SampleDesc.Count = 1;
texDesc.SampleDesc.Quality = 0;
texDesc.Usage = D3D11_USAGE_DEFAULT;
texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
texDesc.CPUAccessFlags = 0;
texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

D3D11_SHADER_RESOURCE_VIEW_DESC SMViewDesc;
SMViewDesc.Format = texDesc.Format;
SMViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
SMViewDesc.TextureCube.MipLevels =  texDesc.MipLevels;
SMViewDesc.TextureCube.MostDetailedMip = 0;

D3D11_SUBRESOURCE_DATA pData[6];
std::vector<vector4b> d[6]; // 6 images of type vector4b = 4 * unsigned char

for (int cubeMapFaceIndex = 0; cubeMapFaceIndex < 6; cubeMapFaceIndex++)
{   
    d[cubeMapFaceIndex].resize(description.width * description.height);

    // fill with red color  
    std::fill(
        d[cubeMapFaceIndex].begin(), 
        d[cubeMapFaceIndex].end(), 
        vector4b(255,0,0,255));

    pData[cubeMapFaceIndex].pSysMem = &d[cubeMapFaceIndex][0];// description.data;
    pData[cubeMapFaceIndex].SysMemPitch = description.width * 4;
    pData[cubeMapFaceIndex].SysMemSlicePitch = 0;
}

HRESULT hr = renderer->getDevice()->CreateTexture2D(&texDesc, 
    description.data[0] ? &pData[0] : nullptr, &m_pCubeTexture);
assert(hr == S_OK);

hr = renderer->getDevice()->CreateShaderResourceView(
    m_pCubeTexture, &SMViewDesc, &m_pShaderResourceView);
assert(hr == S_OK);
	*/
}

// -----------------------------------------------------------------------------------------------
S_API size_t DX11Texture::BitsPerPixel(REFGUID targetGuid, IWICImagingFactory* pWIC)
{
	if (!pWIC)
		return 0;

	ScopedTextureLoadingObject<IWICComponentInfo> comInfo;
	if (Failure(pWIC->CreateComponentInfo(targetGuid, &comInfo)))
		return 0;

	// make sure we got the correct component type
	WICComponentType comType;
	if (Failure(comInfo->GetComponentType(&comType)))
		return 0;
	if (comType != WICPixelFormat)
		return 0;

	ScopedTextureLoadingObject<IWICPixelFormatInfo> pxlFmtInfo;
	if (Failure(comInfo->QueryInterface(__uuidof(IWICPixelFormatInfo), reinterpret_cast<void**>(&pxlFmtInfo))))
		return 0;

	UINT bpp;
	if (Failure(pxlFmtInfo->GetBitsPerPixel(&bpp)))
		return 0;

	return bpp;
}

// -----------------------------------------------------------------------------------------------
S_API unsigned int DX11Texture::GetSubresourceIndex(unsigned int mipLevel, unsigned int arraySlice /*= 0*/)
{
	// DX stores a contiguous array of mipmap subresources.
	//    [ 0: Mip0,  1: Mip1 ] [ 2: Mip0, 3: Mip1 ] ...

	mipLevel = min(mipLevel, m_DXTextureDesc.MipLevels - 1);
	arraySlice = min(arraySlice, m_DXTextureDesc.ArraySize - 1);

	unsigned int nMipLevels = m_DXTextureDesc.MipLevels;
	if (nMipLevels == 0)
		nMipLevels = 1;

	return arraySlice * nMipLevels + mipLevel;
}




// -----------------------------------------------------------------------------------------------
// remember that the w and h parameters will specify the output texture size to which the image will be scaled to
S_API SResult DX11Texture::LoadFromFile(const string& cFileName, unsigned int w /*=0*/, unsigned int h /*=0*/, unsigned int mipLevels /*=0*/)
{
	Clear();

	if (!IS_VALID_PTR(m_pDXRenderer))
		return CLog::Log(S_ERROR, "DX11Texture::LoadFromFile('%s'): Renderer not initialized", cFileName.c_str());

	SResult res;
	HRESULT hRes;

	// ----------------------------------------------------------------------------------------------------------------------
	
	SLoadedTextureBitmap bitmap;
	memset(&bitmap, 0, sizeof(bitmap));
	bitmap.width = w;
	bitmap.height = h;

	res = LoadTextureImage(cFileName, bitmap);
	if (Failure(res))
	{
		return CLog::Log(S_ERROR, "DX11Texture::LoadFromFile('%s'): Failed LoadTextureImage()", cFileName.c_str());
	}


	// ----------------------------------------------------------------------------------------------------------------------
	// Check if autogeneration of mip levels is supported

	bool bMipAutoGenSupported = CheckMipMapAutogenSupported(bitmap.format);

	// ----------------------------------------------------------------------------------------------------------------------
	// Now create the directx texture
	
	D3D11_TEXTURE2D_DESC& textureDesc = m_DXTextureDesc;
	textureDesc.ArraySize = 1;
	textureDesc.BindFlags = (bMipAutoGenSupported) ? (D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET) : (D3D11_BIND_SHADER_RESOURCE);
	textureDesc.CPUAccessFlags = (m_bDynamic) ? D3D11_CPU_ACCESS_WRITE /* | D3D11_CPU_ACCESS_READ */ : 0;
	textureDesc.Format = bitmap.format;
	textureDesc.Width = bitmap.width;
	textureDesc.Height = bitmap.height;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0; // No MS for now!

	if (!m_bDynamic)
	{
		textureDesc.MiscFlags = (bMipAutoGenSupported) ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;
		textureDesc.BindFlags = (bMipAutoGenSupported) ? (D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET) : (D3D11_BIND_SHADER_RESOURCE);
		textureDesc.CPUAccessFlags = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.MipLevels = (bMipAutoGenSupported) ? 0 : 1;
	}
	else
	{
		textureDesc.MiscFlags = 0;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		textureDesc.Usage = D3D11_USAGE_DYNAMIC;
		textureDesc.MipLevels = 1;
	}	
	
	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = (void*)bitmap.buffer;
	initData.SysMemPitch = static_cast<UINT>(bitmap.imageStride);
	initData.SysMemSlicePitch = static_cast<UINT>(bitmap.imageSize);
	
	m_pDXTexture = nullptr;

	// Create the texture
	hRes = m_pDXRenderer->GetD3D11Device()->CreateTexture2D(&textureDesc, (!m_bDynamic && bMipAutoGenSupported) ? nullptr : &initData, &m_pDXTexture);

	if (Failure(hRes) || m_pDXTexture == nullptr)
	{
		return CLog::Log(S_ERROR, "Failed to create DirectX11 Texture for '%s' (%s)!", cFileName.c_str(), m_Specification.c_str());
	}

#ifdef _DEBUG
	const string& nm = m_Specification;
	m_pDXTexture->SetPrivateData(WKPDID_D3DDebugObjectName, nm.length(), nm.c_str());
#endif


	// ----------------------------------------------------------------------------------------------------------------------
	// Create the Shader Resource View

	memset(&m_DXSRVDesc, 0, sizeof(m_DXSRVDesc));
	m_DXSRVDesc.Format = bitmap.format;
	m_DXSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	
	m_DXSRVDesc.Texture2D.MostDetailedMip = 0;
	if (bMipAutoGenSupported)
		m_DXSRVDesc.Texture2D.MipLevels = (mipLevels == 0 ? -1 : mipLevels);
	else
		m_DXSRVDesc.Texture2D.MipLevels = 1;

	hRes = m_pDXRenderer->GetD3D11Device()->CreateShaderResourceView(m_pDXTexture, &m_DXSRVDesc, &m_pDXSRV);

	if (Failure(hRes))
	{
		m_pDXTexture->Release();
		return CLog::Log(S_ERROR, "Failed create shader resource view for texture!");
	}

	if (!m_bDynamic && bMipAutoGenSupported)
	{
		ID3D11DeviceContext* pDXDevCon = m_pDXRenderer->GetD3D11DeviceContext();
		pDXDevCon->UpdateSubresource(m_pDXTexture, 0, nullptr, bitmap.buffer, static_cast<UINT>(bitmap.imageStride), static_cast<UINT>(bitmap.imageSize));
		pDXDevCon->GenerateMips(m_pDXSRV);
	}

	// ----------------------------------------------------------------------------------------------------------------------
	// Determine type

	MapTextureType(bitmap.format, m_Type, true);

	// ----------------------------------------------------------------------------------------------------------------------
	// Store staged data

	if (m_bStaged)
	{
		if (IS_VALID_PTR(m_pStagedData))
			free(m_pStagedData);

		m_pStagedData = malloc(bitmap.imageSize);
		memcpy(m_pStagedData, bitmap.buffer, bitmap.imageSize);
	}	

	CLog::Log(S_DEBUG, "Loaded texture %s", cFileName.c_str());

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Texture::LoadArraySliceFromFile(unsigned int i, const string& filePath)
{
	HRESULT hr;
	SResult res;

	if (!m_pDXTexture)
		return CLog::Log(S_ERROR, "Cannot LoadArraySliceFromFile(): Texture not initialized (%s)", m_Specification.c_str());

	if (!m_bArray)
		return CLog::Log(S_ERROR, "Cannot load array slice in non-array texture (%s)", m_Specification.c_str());

	if (i > m_DXTextureDesc.ArraySize)
		return CLog::Log(S_ERROR, "Cannot load texture array slice %d: Only initialized with %d slices (%s)", i, m_DXTextureDesc.ArraySize, m_Specification.c_str());

	// Load image
	SLoadedTextureBitmap bitmap;
	memset(&bitmap, 0, sizeof(bitmap));
	bitmap.width = m_DXTextureDesc.Width;
	bitmap.height = m_DXTextureDesc.Height;
	bitmap.format = m_DXTextureDesc.Format;

	res = LoadTextureImage(filePath, bitmap);
	
	if (Failure(res))
		return CLog::Log(S_ERROR, "Failed load texture array slice image '%s' (%s)", filePath.c_str(), m_Specification.c_str());

	if (bitmap.format != m_DXTextureDesc.Format)
		return CLog::Log(S_ERROR, "DX11Texture(%s)::LoadArraySliceFromFile(%s): Loaded format incompatible with array format: (loaded: %s, array: %s)",
			m_Specification.c_str(), filePath.c_str(), GetDXGIFormatName(bitmap.format), GetDXGIFormatName(m_DXTextureDesc.Format));

	// Fill slice in staging texture
	ID3D11DeviceContext* pD3DDevCtx = m_pDXRenderer->GetD3D11DeviceContext();

	unsigned int iSubresource = GetSubresourceIndex(0, i);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	hr = pD3DDevCtx->Map(m_pDXStagingTexture, iSubresource, D3D11_MAP_WRITE, 0, &mappedResource);
	if (FAILED(hr))
	{
		return CLog::Log(S_ERROR, "DX11Texture(%s)::LoadArraySliceFromFile(%s): Failed map staging texture subresource #%d",
			m_Specification.c_str(), filePath.c_str(), iSubresource);
	}

	for (unsigned int y = 0; y < bitmap.height; ++y)
	{
		memcpy((char*)mappedResource.pData + y * mappedResource.RowPitch,
			bitmap.buffer + y * bitmap.imageStride,
			bitmap.width * bitmap.bytePerPixel);
	}

	pD3DDevCtx->Unmap(m_pDXStagingTexture, iSubresource);

	// Copy subresource from staging texture to live texture
	pD3DDevCtx->CopySubresourceRegion(m_pDXTexture, iSubresource, 0, 0, 0, m_pDXStagingTexture, iSubresource, NULL);

	// Update mips
	if (m_DXTextureDesc.MiscFlags & D3D11_RESOURCE_MISC_GENERATE_MIPS)
	{
		if (m_pDXSRV)
			pD3DDevCtx->GenerateMips(m_pDXSRV);
		else
			CLog::Log(S_WARN, "Could not generate mipmaps in LoadArraySliceFromFile(): SRV not initialized (%s)", m_Specification.c_str());
	}

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Texture::CopyArraySliceFromTexture(unsigned int i, const ITexture* pSrcTex)
{
	HRESULT hr;

	if (!m_pDXTexture || !m_pDXStagingTexture || !m_bArray || !m_bDynamic)
		return CLog::Log(S_ERROR, "Failed CopyArraySliceFromTexture(): Texture is not a dynamic array or not initialized at all (%s)", m_Specification.c_str());

	const DX11Texture* pDXSrcTex = dynamic_cast<const DX11Texture*>(pSrcTex);
	if (i >= m_DXTextureDesc.ArraySize || !pDXSrcTex || pDXSrcTex->IsArray())
		return CLog::Log(S_INVALIDPARAM, "Failed CopyArraySliceFromTexture(): Invalid parameters (%s)", m_Specification.c_str());

	if (pDXSrcTex->m_bLocked)
		return CLog::Log(S_ERROR, "Failed CopyArraySliceFromTexture(): Source texture (%s) is locked! (%s)", pDXSrcTex->GetSpecification().c_str(), m_Specification.c_str());

	if (pDXSrcTex->GetType() != m_Type)
		return CLog::Log(S_ERROR, "Failed CopyArraySliceFromTexture(): Source texture type (%s) incompatible with %s! (%s)",
			GetTextureTypeName(pDXSrcTex->GetType()), GetTextureTypeName(m_Type), m_Specification.c_str());

	const D3D11_TEXTURE2D_DESC& srcTexDesc = pDXSrcTex->m_DXTextureDesc;
	if (srcTexDesc.Width != m_DXTextureDesc.Width || srcTexDesc.Height != m_DXTextureDesc.Height)
		return CLog::Log(S_ERROR, "Failed CopyArraySliceFromTexture(): Source texture size (%dx%d) incompatible with %dx%d! (%s)",
			srcTexDesc.Width, srcTexDesc.Height, m_DXTextureDesc.Width, m_DXTextureDesc.Height, m_Specification.c_str());

	if (!pDXSrcTex->m_bStaged && pDXSrcTex->m_pStagedData)
		return CLog::Log(S_ERROR, "Failed CopyArraySliceFromTexture(): Source texture (%s) is not staged/readable! (%s)", pDXSrcTex->GetSpecification().c_str(), m_Specification.c_str());
	
	// Copy source texture to staging texture
	ID3D11DeviceContext* pCtx = m_pDXRenderer->GetD3D11DeviceContext();
	unsigned int iSubresource = GetSubresourceIndex(0, i);
	
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	hr = pCtx->Map(m_pDXStagingTexture, iSubresource, D3D11_MAP_WRITE, 0, &mappedResource);
	if (FAILED(hr))
		return CLog::Log(S_ERROR, "Failed CopyArraySliceFromTexture(): Failed map staging texture (%s)", m_Specification.c_str());

	unsigned int srcRowPitch = GetTextureBPP(pDXSrcTex->GetType()) * srcTexDesc.Width;

	for (unsigned int y = 0; y < m_DXTextureDesc.Height; ++y)
	{
		memcpy((char*)mappedResource.pData + y * mappedResource.RowPitch,
			(char*)pDXSrcTex->m_pStagedData + y * srcRowPitch,
			mappedResource.RowPitch);
	}

	pCtx->Unmap(m_pDXStagingTexture, iSubresource);

	// Upload staging texture contents to live texture
	pCtx->CopySubresourceRegion(m_pDXTexture, iSubresource, 0, 0, 0, m_pDXStagingTexture, iSubresource, 0);

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Texture::ResizeArray(unsigned int count)
{
	HRESULT hr;
	SResult sr;

	if (!m_pDXTexture || !m_pDXStagingTexture || !m_bArray)
		return CLog::Log(S_ERROR, "Failed ResizeArray(): Texture is not an array or not initialized at all (%s)", m_Specification.c_str());

	if (count == 0)
		return CLog::Log(S_ERROR, "Failed ResizeArray(): count must be at least 1 (was %d, %s)", count, m_Specification.c_str());

	if (count == m_DXTextureDesc.ArraySize)
		return S_SUCCESS;

	if (count >= 100)
		CLog::Log(S_WARN, "DX11Texture::ResizeArray(): New count (=%d) is extraordinarily big!", count);

	// Copy old data into new initial subresource data
	ID3D11Device* pD3DDevice = m_pDXRenderer->GetD3D11Device();
	ID3D11DeviceContext* pD3DDeviceContext = m_pDXRenderer->GetD3D11DeviceContext();

	D3D11_SUBRESOURCE_DATA *data = new D3D11_SUBRESOURCE_DATA[count];
	
	unsigned int bytePerPixel = GetTextureBPP(m_Type);
	if (bytePerPixel == 0)
		CLog::Log(S_WARN, "In ResizeArray(): bytePerPixel=0 for texture type='%s'", GetTextureTypeName(m_Type));

	unsigned int sliceSz = m_DXTextureDesc.Width * m_DXTextureDesc.Height * bytePerPixel;
	unsigned int sliceRowPitch = m_DXTextureDesc.Width * bytePerPixel;
	unsigned int nCopiedSubresources = min(count, m_DXTextureDesc.ArraySize);
	for (unsigned int i = 0; i < count; ++i)
	{
		if (i < nCopiedSubresources)
		{
			unsigned int iSubresource = GetSubresourceIndex(0, i);

			D3D11_MAPPED_SUBRESOURCE slice;
			hr = pD3DDeviceContext->Map(m_pDXStagingTexture, iSubresource, D3D11_MAP_READ, 0, &slice);
			if (FAILED(hr))
			{
				CLog::Log(S_ERROR, "Failed map staging array slice in ResizeArray(%d), %s", count, m_Specification.c_str());
				continue;
			}

			ZeroMemory(&data[i], sizeof(data[i]));
			data[i].pSysMem = new char[sliceSz];
			data[i].SysMemPitch = sliceRowPitch;
			for (unsigned int row = 0; row < m_DXTextureDesc.Height; ++row)
			{
				memcpy((char*)data[i].pSysMem + data[i].SysMemPitch * row,
					(const char*)slice.pData + slice.RowPitch * row,
					data[i].SysMemPitch);
			}

			pD3DDeviceContext->Unmap(m_pDXStagingTexture, iSubresource);
		}
		else
		{
			data[i].pSysMem = new char[sliceSz]; // zero
			data[i].SysMemPitch = sliceRowPitch;
		}
	}

	// Release old texture
	m_pDXStagingTexture->Release();
	m_pDXStagingTexture = 0;
	
	m_pDXTexture->Release();
	m_pDXTexture = 0;

	if (m_pDXSRV)
	{
		m_pDXSRV->Release();
		m_pDXSRV = 0;
	}

	// Create new array texture
	m_DXTextureDesc.ArraySize = count;
	hr = pD3DDevice->CreateTexture2D(&m_DXTextureDesc, data, &m_pDXTexture);
	
	if (FAILED(hr))
	{
		for (unsigned int i = 0; i < count; ++i)
			delete[] data[i].pSysMem;
		delete[] data;

		return CLog::Log(S_ERROR, "Failed ResizeArray(count=%d): Failed create new texture array (%s)", m_Specification.c_str());
	}

	// Create new staging texture
	sr = CreateArrayStagingTexture(data);

	for (unsigned int i = 0; i < count; ++i)
		delete[] data[i].pSysMem;
	delete[] data;

	if (Failure(sr))
		return S_ERROR;

	// Create SRV
	m_DXSRVDesc.Texture2DArray.ArraySize = m_DXTextureDesc.ArraySize;
	hr = pD3DDevice->CreateShaderResourceView(m_pDXTexture, &m_DXSRVDesc, &m_pDXSRV);
	if (FAILED(hr))
		return CLog::Log(S_ERROR, "Failed ResizeArray(count=%d): Failed create SRV (%s)", count, m_Specification.c_str());

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API bool DX11Texture::CheckMipMapAutogenSupported(DXGI_FORMAT format)
{
	if (!m_pDXRenderer)
		return false;
	
	UINT fmtSupport = 0;
	HRESULT hRes = m_pDXRenderer->GetD3D11Device()->CheckFormatSupport(format, &fmtSupport);
	return Success(hRes) && (fmtSupport & D3D11_FORMAT_SUPPORT_MIP_AUTOGEN);
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Texture::CreateArrayStagingTexture(const D3D11_SUBRESOURCE_DATA* pInitialData /*= 0*/)
{
	HRESULT hr;

	if (m_pDXStagingTexture)
	{
		m_pDXStagingTexture->Release();
		m_pDXStagingTexture = 0;
	}

	D3D11_TEXTURE2D_DESC stagingTexDesc;
	memcpy(&stagingTexDesc, &m_DXTextureDesc, sizeof(stagingTexDesc));
	stagingTexDesc.Usage = D3D11_USAGE_STAGING;
	stagingTexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	stagingTexDesc.BindFlags = 0;
	stagingTexDesc.MiscFlags = 0;

	hr = m_pDXRenderer->GetD3D11Device()->CreateTexture2D(&stagingTexDesc, pInitialData, &m_pDXStagingTexture);

	if (Failure(hr) || !m_pDXStagingTexture)
		return CLog::Log(S_ERROR, "Failed to create staging texture for dynamic texture array (%s)", m_Specification.c_str());

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Texture::CreateEmptyIntrnl(unsigned int arraySize, unsigned int w, unsigned int h, unsigned int mipLevels, ETextureType type, SColor clearcolor)
{
	HRESULT hr;

	Clear();

	if (!IS_VALID_PTR(m_pDXRenderer))
		return CLog::Log(S_ERROR, "DX11Texture::CreateEmpty(): Renderer not initialized");

	m_Type = type;

	
	// !dynamic && !staged	-->	tex w/ D3D_USAGE_DEFAULT
	//  dynamic && !staged  --> tex w/ D3D_USAGE_DYNAMIC and D3D_CPU_ACCESS_WRITE
	//  dynamic &&  staged	-->	tex w/ D3D_USAGE_DEFAULT + staging tex w/ D3D_USAGE_STAGING and D3D_CPU_ACCESS_READ|WRITE
	// !dynamic &&  staged	--> tex w/ D3D_USAGE_DEFAULT + staging tex w/ D3D_USAGE_STAGING and D3D_CPU_ACCESS_READ

	m_bDynamic = true; // TODO ?
	m_bStaged = true;

	// Fill texture desc
	m_DXTextureDesc.ArraySize = arraySize;
	m_DXTextureDesc.Width = w;
	m_DXTextureDesc.Height = h;
	m_DXTextureDesc.SampleDesc.Count = 1;
	m_DXTextureDesc.SampleDesc.Quality = 0; // No MS for now!	

	MapTextureType(m_DXTextureDesc.Format, type, false);

	bool bMipAutoGenSupported = CheckMipMapAutogenSupported(m_DXTextureDesc.Format);

	if (!m_bDynamic)
	{
		m_DXTextureDesc.MiscFlags = (bMipAutoGenSupported) ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;
		m_DXTextureDesc.BindFlags = (bMipAutoGenSupported) ? (D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET) : (D3D11_BIND_SHADER_RESOURCE);
		m_DXTextureDesc.CPUAccessFlags = 0;
		m_DXTextureDesc.Usage = D3D11_USAGE_DEFAULT;
		m_DXTextureDesc.MipLevels = (bMipAutoGenSupported) ? 0 : 1;
	}
	else
	{
		m_DXTextureDesc.MiscFlags = 0;
		m_DXTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		m_DXTextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		m_DXTextureDesc.MipLevels = 1;
		m_DXTextureDesc.Usage = D3D11_USAGE_DYNAMIC;
	}

	if (m_bDynamic && m_DXTextureDesc.ArraySize > 1)
	{
		if (bMipAutoGenSupported)
		{
			m_DXTextureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
			m_DXTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			m_DXTextureDesc.MipLevels = mipLevels;
		}

		// We'll use a staging texture
		m_DXTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	}

	// Fill texture with clear color
	unsigned int nPixels = w * h;
	D3D11_SUBRESOURCE_DATA* initData = new D3D11_SUBRESOURCE_DATA[m_DXTextureDesc.ArraySize];
	switch (type)
	{
	case eTEXTURE_R8G8B8A8_UNORM:
		{
			for (unsigned int iSlice = 0; iSlice < m_DXTextureDesc.ArraySize; ++iSlice)
			{
				char* pEmptyPixels = new char[nPixels * 4];
				for (unsigned int iPxl = 0; iPxl < nPixels * 4; iPxl += 4)
				{
					pEmptyPixels[iPxl] = (char)(clearcolor.r * 255.0f);
					pEmptyPixels[iPxl + 1] = (char)(clearcolor.g * 255.0f);
					pEmptyPixels[iPxl + 2] = (char)(clearcolor.b * 255.0f);
					pEmptyPixels[iPxl + 3] = (char)(clearcolor.a * 255.0f);
				}

				initData[iSlice].pSysMem = pEmptyPixels;
				initData[iSlice].SysMemPitch = w * 4;
				initData[iSlice].SysMemSlicePitch = nPixels * 4;
			}

			break;
		}

	case eTEXTURE_D32_FLOAT:
	case eTEXTURE_R32_FLOAT:
		{
			for (unsigned int iSlice = 0; iSlice < m_DXTextureDesc.ArraySize; ++iSlice)
			{
				float* pEmptyPixels = new float[nPixels];
				for (unsigned int iPxl = 0; iPxl < nPixels; ++iPxl)
					pEmptyPixels[iPxl] = clearcolor.r;

				initData[iSlice].pSysMem = pEmptyPixels;
				initData[iSlice].SysMemPitch = w * sizeof(float);
				initData[iSlice].SysMemSlicePitch = nPixels * sizeof(float);
			}

			break;
		}

	default:
		CLog::Log(S_WARNING, "Cannot fill new empty texture: Type not supported (%s)", GetTextureTypeName(type));
		break;
	}

	// Create texture
	m_pDXTexture = nullptr;

	hr = m_pDXRenderer->GetD3D11Device()->CreateTexture2D(&m_DXTextureDesc, (bMipAutoGenSupported ? nullptr : initData), &m_pDXTexture);

	if (Failure(hr) || m_pDXTexture == nullptr)
		return CLog::Log(S_ERROR, "Failed to create empty DirectX11 Texture (CreateTexture2D failed)!");

#ifdef _DEBUG
	const string& nm = m_Specification;
	m_pDXTexture->SetPrivateData(WKPDID_D3DDebugObjectName, nm.length(), nm.c_str());
#endif

	// Create staging texture for dynamic texture arrays
	if (m_bDynamic && m_DXTextureDesc.ArraySize > 1)
	{
		CreateArrayStagingTexture();
	}

	// Create SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	memset(&srvDesc, 0, sizeof(srvDesc));
	srvDesc.Format = m_DXTextureDesc.Format;
	
	unsigned int numMips = 1;
	if (bMipAutoGenSupported)
		numMips = (mipLevels == 0 ? -1 : mipLevels);
	
	if (m_DXTextureDesc.ArraySize > 1)
	{
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		srvDesc.Texture2DArray.ArraySize = m_DXTextureDesc.ArraySize;
		srvDesc.Texture2DArray.FirstArraySlice = 0;
		srvDesc.Texture2DArray.MipLevels = numMips;
	}
	else
	{
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = numMips;
	}

	if (Failure(m_pDXRenderer->GetD3D11Device()->CreateShaderResourceView(m_pDXTexture, &srvDesc, &m_pDXSRV)))
	{
		m_pDXTexture->Release();
		return CLog::Log(S_ERROR, "Failed create shader resource view for empty texture!");
	}

	// Determine actual mip level count
	// This gives us the correct mip level size when previously specifying 0 to create a full mip chain
	D3D11_TEXTURE2D_DESC postDesc;
	m_pDXTexture->GetDesc(&postDesc);

	m_DXTextureDesc.MipLevels = postDesc.MipLevels;


	// Generate mips if possible
	ID3D11DeviceContext* pDXDevCon = m_pDXRenderer->GetD3D11DeviceContext();
	if (!m_bDynamic && bMipAutoGenSupported)
	{
		// Is this necessary here??
		for (unsigned int i = 0; i < m_DXTextureDesc.ArraySize; ++i)
			pDXDevCon->UpdateSubresource(m_pDXTexture, i, nullptr, initData[i].pSysMem, initData[i].SysMemPitch, initData[i].SysMemSlicePitch);

		pDXDevCon->GenerateMips(m_pDXSRV);
	}
	else if (m_bDynamic)
	{
		//pDXDevCon->GenerateMips(m_pDXSRV);
	}

	// Store staging data
	m_pStagedData = 0;
	if (m_bStaged && !m_bArray)
	{
		m_pStagedData = malloc(initData[0].SysMemSlicePitch);
		memcpy(m_pStagedData, initData[0].pSysMem, initData[0].SysMemSlicePitch);
	}

	
	for (unsigned int i = 0; i < m_DXTextureDesc.ArraySize; ++i)
		delete[] initData[i].pSysMem;
	delete[] initData;
	initData = 0;

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Texture::CreateEmpty(unsigned int w, unsigned int h, unsigned int mipLevels, ETextureType type, SColor clearcolor)
{	
	return CreateEmptyIntrnl(1, w, h, mipLevels, type, clearcolor);
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Texture::CreateEmptyArray(unsigned int count, unsigned int w, unsigned int h,
	ETextureType type, unsigned int mipLevels, SColor clearcolor)
{
	SResult r;
	r = CreateEmptyIntrnl(count, w, h, mipLevels, type, clearcolor);
	if (Failure(r))
		return r;

	m_bArray = true;
	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Texture::D3D11_InitializeFromExistingResource(ID3D11Texture2D* pResource, DXGI_FORMAT format /*= DXGI_FORMAT_UNKNOWN*/)
{
	if (!pResource)
		return CLog::Log(S_ERROR, "DX11Texture::D3D11_InitializeFromExistingResource(): Invalid resource given");

	Clear();

	m_pDXTexture = pResource;
	m_pDXTexture->AddRef();
	m_pDXTexture->GetDesc(&m_DXTextureDesc);

	MapTextureType(m_DXTextureDesc.Format, m_Type, true);
	m_bDynamic = false;
	m_bStaged = false;

	// Create SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	memset(&srvDesc, 0, sizeof(srvDesc));
	srvDesc.Format = (format == DXGI_FORMAT_UNKNOWN ? m_DXTextureDesc.Format : format);
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;		// maybe someday use MS?
	srvDesc.Texture2D.MipLevels = 1;

	if (Failure(m_pDXRenderer->GetD3D11Device()->CreateShaderResourceView(m_pDXTexture, &srvDesc, &m_pDXSRV)))
		return CLog::Log(S_ERROR, "DX11Texture::D3D11_InitializeFromExistingResource(): Failed create SRV");

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Texture::Fill(SColor color)
{
	unsigned int *pPixels, nPixels;
	if (Failure(Lock((void**)&pPixels, &nPixels)))
		return CLog::Log(S_DEBUG, "Failed fill texture: Lock failed");

	for (unsigned int i = 0; i < nPixels; ++i)
	{
		pPixels[i] = color.ToInt_RGBA();
	}

	return Unlock();
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Texture::FillArraySlice(unsigned int iSlice, const SColor& color)
{
	HRESULT hr;

	if (!m_pDXTexture)
		return CLog::Log(S_ERROR, "Failed DX11Texture::FillArraySlice(): Texture not initialized (%s)", m_Specification.c_str());

	if (!m_bArray)
		return CLog::Log(S_ERROR, "Failed DX11Texture::FillArraySlice(): Texture not an array texture (%s)", m_Specification.c_str());

	if (!m_pDXStagingTexture)
		return CLog::Log(S_ERROR, "Failed DX11Texture::FillArraySlice(): Staging texture for dynamic texture array not initialized (%s)", m_Specification.c_str());

	if (iSlice >= m_DXTextureDesc.ArraySize)
		return CLog::Log(S_INVALIDPARAM, "Failed DX11Texture::FillArraySlice(%d): Array only has %d slices (%s)",
			iSlice, m_DXTextureDesc.ArraySize, m_Specification.c_str());

	unsigned int iSubresource = GetSubresourceIndex(0, iSlice);

	ID3D11DeviceContext* pD3DDevCtx = m_pDXRenderer->GetD3D11DeviceContext();
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	memset(&mappedSubresource, 0, sizeof(D3D11_MAPPED_SUBRESOURCE));
	hr = pD3DDevCtx->Map(m_pDXStagingTexture, iSubresource, D3D11_MAP_WRITE, 0, &mappedSubresource);
	if (FAILED(hr))
		return CLog::Log(S_ERROR, "Failed DX11Texture::FillArraySlice(): Failed map array slice subresource (%s)", m_Specification.c_str());

	unsigned int bytePerPixel = GetTextureBPP(m_Type);
	char* pRowData = new char[m_DXTextureDesc.Width * bytePerPixel];
	unsigned int clearColorUInt = color.ToInt_RGBA();
	for (unsigned int x = 0; x < m_DXTextureDesc.Width; ++x)
	{
		switch (m_Type)
		{
		case eTEXTURE_R8G8B8A8_UNORM:
			{
				((unsigned int*)pRowData)[x] = clearColorUInt;
				break;
			}
		case eTEXTURE_R32_FLOAT:
		case eTEXTURE_D32_FLOAT:
			{
				((float*)pRowData)[x] = color.r;
				break;
			}
		case eTEXTURE_R16G16_FLOAT:
			{
				float* pFloatRowData = (float*)pRowData;
				pFloatRowData[x * 2] = color.r;
				pFloatRowData[x * 2 + 1] = color.g;
				break;
			}
		default:
			{
				for (unsigned int i = 0; i < bytePerPixel; ++i)
				{
					if (i < 4)
						pRowData[x + i] = ((char*)&clearColorUInt)[i];
					else
						pRowData[x + i] = 0;
				}
				break;
			}
		}
	}

	for (unsigned int y = 0; y < m_DXTextureDesc.Height; ++y)
	{
		memcpy((char*)mappedSubresource.pData + y * mappedSubresource.RowPitch, pRowData, m_DXTextureDesc.Width * bytePerPixel);
	}

	pD3DDevCtx->Unmap(m_pDXStagingTexture, iSubresource);

	// Copy staged contents to live texture
	pD3DDevCtx->CopySubresourceRegion(m_pDXTexture, iSubresource, 0, 0, 0, m_pDXStagingTexture, iSubresource, NULL);

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API bool DX11Texture::IsInitialized() const
{
	return (m_RefCount > 0 && IS_VALID_PTR(m_pDXTexture) && IS_VALID_PTR(m_pDXSRV));
}

// -----------------------------------------------------------------------------------------------
S_API const string& DX11Texture::GetSpecification(void) const
{
	return m_Specification;
}

// -----------------------------------------------------------------------------------------------
S_API void DX11Texture::SetSpecification(const string& specification)
{
	m_Specification = specification;

#ifdef _DEBUG
	if (m_pDXTexture)
		m_pDXTexture->SetPrivateData(WKPDID_D3DDebugObjectName, m_Specification.length(), m_Specification.c_str());
#endif
}

// -----------------------------------------------------------------------------------------------
S_API ETextureType DX11Texture::GetType() const
{
	return m_Type;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Texture::GetSize(unsigned int* pW, unsigned int* pH)
{
	if (!IS_VALID_PTR(m_pDXTexture))
		return S_NOTINIT;

	if (IS_VALID_PTR(pW))
		*pW = m_DXTextureDesc.Width;

	if (IS_VALID_PTR(pH))
		*pH = m_DXTextureDesc.Height;

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API unsigned int DX11Texture::GetArraySize() const
{
	if (!m_bArray || !m_pDXTexture)
		return 0;

	return m_DXTextureDesc.ArraySize;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Texture::Lock(void **pPixels, unsigned int* pnPixels, unsigned int* pnRowPitch /*= 0*/, unsigned int iArraySlice /*= 0*/)
{
	HRESULT hr;

	if (!IS_VALID_PTR(m_pDXRenderer))
		return CLog::Log(S_ERROR, "DX11Texture::Lock(): Renderer not initialized");

	if (!m_bDynamic)
		return CLog::Log(S_ERROR, "Tried DX11Texture::Lock on non-dynamic texture (%s)", m_Specification.c_str());

	if (m_bLocked)
		return CLog::Log(S_ERROR, "Cannot lock DX11Texture (%s): Already locked!", m_Specification.c_str());

	if (!IS_VALID_PTR(pPixels) || !IS_VALID_PTR(pnPixels))
		return S_INVALIDPARAM;

	if (!IsInitialized())
		return S_NOTINIT;

	ID3D11DeviceContext* pDXDevCon = m_pDXRenderer->GetD3D11DeviceContext();
	if (!IS_VALID_PTR(pDXDevCon))
		return S_NOTINIT;
	
	unsigned int bytePerLockedPixel = GetTextureBPP(m_Type);

	if (m_bStaged && !m_bArray)
	{
		if (!IS_VALID_PTR(m_pStagedData))
			return CLog::Log(S_ERROR, "Failed lock staged texture (%s): Staged data invalid (0x%p)!", m_Specification.c_str(), m_pStagedData);

		m_bLocked = true;
		*pPixels = m_pStagedData;
		m_pLockedData = 0;

		if (IS_VALID_PTR(pnRowPitch))
			*pnRowPitch = m_DXTextureDesc.Width * bytePerLockedPixel;
	}
	else if (m_bArray)
	{
		if (!m_pDXStagingTexture)
			return CLog::Log(S_ERROR, "Failed lock dynamic array texture: Staging texture not initialized (%s)", m_Specification.c_str());

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		ZeroMemory(&mappedResource, sizeof(mappedResource));
		
		unsigned int iSubresource = GetSubresourceIndex(0, iArraySlice);

		hr = pDXDevCon->Map(m_pDXStagingTexture, iSubresource, D3D11_MAP_READ_WRITE, 0, &mappedResource);
		if (FAILED(hr))
			return CLog::Log(S_ERROR, "DX11Texture::Lock(): Failed map array staging texture (%s)", m_Specification.c_str());

		m_bLocked = true;
		*pPixels = mappedResource.pData;
		m_pLockedData = 0;

		if (pnRowPitch)
			*pnRowPitch = mappedResource.RowPitch;
	}
	else
	{
		return CLog::Log(S_ERROR, "Cannot lock non-staged and non-array texture (%s)", m_Specification.c_str());
	}

	*pnPixels = m_DXTextureDesc.Width * m_DXTextureDesc.Height;	
	m_nLockedBytes = (*pnPixels) * bytePerLockedPixel;

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Texture::Unlock(SRectangle* pUpdateRect /*= 0*/, unsigned int iArraySlice /*= 0*/)
{
	if (!IS_VALID_PTR(m_pDXRenderer))
		return CLog::Log(S_ERROR, "DX11Texture::Unlock(): Renderer not initialized");

	if (!IsInitialized())
		return S_NOTINIT;

	if (!m_bLocked)
		return CLog::Log(S_WARN, "Tried unlock texture (%s) which is not locked!", m_Specification.c_str());

	assert(m_nLockedBytes > 0);

	if (!m_bStaged && !IS_VALID_PTR(m_pLockedData))
		return S_ERROR;

	ID3D11DeviceContext* pDXDevCon = m_pDXRenderer->GetD3D11DeviceContext();
	if (!IS_VALID_PTR(pDXDevCon))
		return S_NOTINIT;

	m_bLocked = false;

	if (m_bStaged && !m_bArray)
	{
		// copy over staged data
		HRESULT hr;
		D3D11_MAPPED_SUBRESOURCE mappedSubresource;
		hr = pDXDevCon->Map(m_pDXTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
		
		if (FAILED(hr))
			return CLog::Log(S_ERROR, "Failed map texture (%s) for staged update!", m_Specification.c_str());

		unsigned int bytePerLockedPixel = GetTextureBPP(m_Type);

		// We have to copy each line separately to fit DX's pitch
		for (unsigned int ln = 0; ln < m_DXTextureDesc.Height; ++ln)
		{
			memcpy((char*)mappedSubresource.pData + ln * mappedSubresource.RowPitch,
				(const char*)m_pStagedData + ln * m_DXTextureDesc.Width * bytePerLockedPixel,
				m_DXTextureDesc.Width * bytePerLockedPixel);
		}

		pDXDevCon->Unmap(m_pDXTexture, 0);
	}
	else if (m_bArray && m_pDXStagingTexture)
	{
		if (iArraySlice >= m_DXTextureDesc.ArraySize)
			return S_INVALIDPARAM;

		unsigned int iSubresource = GetSubresourceIndex(0, iArraySlice);

		pDXDevCon->Unmap(m_pDXStagingTexture, iSubresource);

		// Upload slice to live texture
		if (m_pDXTexture)
		{
			unsigned int dx = 0, dy = 0;
			D3D11_BOX updateBox, *pUpdateBox = 0;
			if (pUpdateRect)
			{
				updateBox.right = min(pUpdateRect->x + pUpdateRect->width, m_DXTextureDesc.Width - 1);
				updateBox.bottom = min(pUpdateRect->y + pUpdateRect->height, m_DXTextureDesc.Height - 1);
				updateBox.left = min(pUpdateRect->x, updateBox.right - 1);
				updateBox.top = min(pUpdateRect->y, updateBox.bottom - 1);
				updateBox.front = 0;
				updateBox.back = 1;

				pUpdateBox = &updateBox;
				dx = updateBox.left;
				dy = updateBox.top;
			}

			pDXDevCon->CopySubresourceRegion(m_pDXTexture, iSubresource, dx, dy, 0, m_pDXStagingTexture, iSubresource, pUpdateBox);
		}
	}

	m_pLockedData = 0;
	m_nLockedBytes = 0;

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Texture::SampleStagedBilinear(Vec2f texcoords, void* pData) const
{
	if (!m_bStaged || !IS_VALID_PTR(m_pStagedData))
		return S_NOTINIT;

	if (!IS_VALID_PTR(pData))
		return S_INVALIDPARAM;

	if (m_Type != eTEXTURE_D32_FLOAT && m_Type != eTEXTURE_R32_FLOAT && m_Type != eTEXTURE_R8G8B8A8_UNORM)
		return S_ERROR;

	Vec2f pixelSizeInTexcoords = 1.0f / Vec2f((float)m_DXTextureDesc.Width, (float)m_DXTextureDesc.Height);

	// Round texcoords to floor
	Vec2f remainder = texcoords % pixelSizeInTexcoords;
	Vec2f roundedTC = texcoords - remainder;

	// transform remainder from [0;pixelSizeInTexcoords] to [0;1]
	remainder /= pixelSizeInTexcoords;		

	// Get all 4 samples
	Vec4f samples[4];
	switch (m_Type)
	{
	case eTEXTURE_D32_FLOAT:
	case eTEXTURE_R32_FLOAT:
	case eTEXTURE_R8G8B8A8_UNORM:
		SampleStaged(roundedTC + Vec2f(-0.5f, -0.5f) * pixelSizeInTexcoords, (void*)&samples[0]);
		SampleStaged(roundedTC + Vec2f( 0.5f, -0.5f) * pixelSizeInTexcoords, (void*)&samples[1]);
		SampleStaged(roundedTC + Vec2f(-0.5f,  0.5f) * pixelSizeInTexcoords, (void*)&samples[2]);		
		SampleStaged(roundedTC + Vec2f( 0.5f,  0.5f) * pixelSizeInTexcoords, (void*)&samples[3]);
		break;
	/*case eTEXTURE_R8G8B8A8_UNORM:
		SampleStaged(roundedTC + Vec2f(-0.5f, -0.5f) * pixelSizeInTexcoords, (void*)&samples[0]);
		SampleStaged(roundedTC + Vec2f( 0.5f, -0.5f) * pixelSizeInTexcoords, (void*)&samples[1]);
		SampleStaged(roundedTC + Vec2f(-0.5f,  0.5f) * pixelSizeInTexcoords, (void*)&samples[2]);
		SampleStaged(roundedTC + Vec2f( 0.5f,  0.5f) * pixelSizeInTexcoords, (void*)&samples[3]);
		break;*/
	default:
		return CLog::Log(S_ERROR, "Cannot SampleStagedBilinear() a texture without floating point format");
	}

	// Interpolate
	Vec4f interpolated[] = 
	{
		Lerp(samples[0], samples[1], remainder.x),
		Lerp(samples[2], samples[3], remainder.x)
	};

	Vec4f out = Lerp(interpolated[0], interpolated[1], remainder.y);

	switch (m_Type)
	{
	case eTEXTURE_D32_FLOAT:
	case eTEXTURE_R32_FLOAT:
		{
			float* pOut = (float*)pData;
			*pOut = out.x;
			break;
		}
	
	case eTEXTURE_R8G8B8A8_UNORM:	
		{
			Vec4f* pOut = (Vec4f*)pData;
			*pOut = out;
			break;
		}

	default:
		return S_ERROR;
	}

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Texture::SampleStaged(const Vec2f& texcoords, void* pData) const
{
	if (!m_bStaged || !IS_VALID_PTR(m_pStagedData))
		return S_NOTINIT;

	if (!IS_VALID_PTR(pData))
		return S_INVALIDPARAM;

	// convert texture coordinates to pixel coordinates
	unsigned int pixelCoords[2];
	pixelCoords[0] = (unsigned int)(texcoords.x * (float)m_DXTextureDesc.Width);
	pixelCoords[1] = (unsigned int)(texcoords.y * (float)m_DXTextureDesc.Height);

	// Apply wrap address mode
	pixelCoords[0] = pixelCoords[0] % m_DXTextureDesc.Width;
	pixelCoords[1] = pixelCoords[1] % m_DXTextureDesc.Height;

	// sample data
	unsigned int bpp;
	switch (m_Type)
	{
	case eTEXTURE_R8G8B8A8_UNORM: bpp = 4; break;
	case eTEXTURE_R32_FLOAT:
	case eTEXTURE_D32_FLOAT:
		bpp = sizeof(float);
	}

	unsigned int bytePos = (pixelCoords[1] * m_DXTextureDesc.Width + pixelCoords[0]) * bpp;
	memcpy(pData, static_cast<char*>(m_pStagedData) + bytePos, bpp);
	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API void* DX11Texture::GetStagedData()
{
	return m_pStagedData;
}

// -----------------------------------------------------------------------------------------------
S_API void DX11Texture::Clear(void)
{
	if (IS_VALID_PTR(m_pStagedData))
		free(m_pStagedData);

	m_pStagedData = 0;

	SP_SAFE_RELEASE(m_pDXSRV);
	SP_SAFE_RELEASE(m_pDXTexture);
	SP_SAFE_RELEASE(m_pDXStagingTexture);

	m_pLockedData = 0;
	m_nLockedBytes = 0;
	m_bLocked = false;

	m_Type = eTEXTURE_R8G8B8A8_UNORM;
	m_bIsCubemap = false;
	m_bArray = false;
}




SP_NMSPACE_END
