/*++

Copyright (C) 2018 3MF Consortium

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL MICROSOFT AND/OR NETFABB BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Abstract:

ExtractInfo.cpp : 3MF Read Example

--*/

#ifndef __GNUC__
#include <tchar.h>
#include <Windows.h>
#endif // __GNUC__

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

// Plain C Includes of 3MF Library
#include "Model/COM/NMR_DLLInterfaces.h"

// Use NMR namespace for the interfaces
using namespace NMR;


HRESULT ShowObjectProperties(_In_ PLib3MFModelObjectResource * pObject)
{
    HRESULT hResult;
    DWORD nNeededChars;
    std::vector<char> pBuffer;
    std::string sName;
    std::string sPartNumber;

    // Retrieve Mesh Name Length
    hResult = lib3mf_object_getnameutf8(pObject, NULL, 0, &nNeededChars);
    if (hResult != LIB3MF_OK)
        return hResult;

    // Retrieve Mesh Name
    if (nNeededChars > 0) {
        pBuffer.resize(nNeededChars + 1);
        hResult = lib3mf_object_getnameutf8(pObject, &pBuffer[0], nNeededChars + 1, NULL);
        pBuffer[nNeededChars] = 0;
        sName = std::string(&pBuffer[0]);
    }

    // Retrieve Mesh Part Number Length
    hResult = lib3mf_object_getpartnumberutf8(pObject, NULL, 0, &nNeededChars);
    if (hResult != LIB3MF_OK)
        return hResult;

    // Retrieve Mesh Name
    if (nNeededChars > 0) {
        pBuffer.resize(nNeededChars + 1);
        hResult = lib3mf_object_getpartnumberutf8(pObject, &pBuffer[0], nNeededChars + 1, NULL);
        pBuffer[nNeededChars] = 0;
        sPartNumber = std::string(&pBuffer[0]);
    }

    // Output Object type
    DWORD ObjectType;
    hResult = lib3mf_object_gettype(pObject, &ObjectType);
    if (hResult != LIB3MF_OK)
        return hResult;

    switch (ObjectType) {
        case MODELOBJECTTYPE_MODEL:
            break;
        case MODELOBJECTTYPE_SUPPORT:
            break;
        case MODELOBJECTTYPE_SOLIDSUPPORT:
            break;
        case MODELOBJECTTYPE_OTHER:
            break;
        default:
            break;

    }


    return LIB3MF_OK;
}



HRESULT ShowMeshObjectInformation(_In_ PLib3MFModelMeshObject * pMeshObject, _In_ PLib3MFModel * pModel)
{
    HRESULT hResult;
    DWORD nVertexCount, nTriangleCount, nBeamCount, nSliceCount = 0;
    DWORD nBeamSetCount;
    BOOL bHasRepresentation;
    DWORD nRepresentationMesh, nClippingMesh;
    eModelBeamLatticeClipMode nClipMode;

    hResult = ShowObjectProperties(pMeshObject);
    if (hResult != LIB3MF_OK)
        return hResult;

    // Retrieve Mesh Vertex Count
    hResult = lib3mf_meshobject_getvertexcount(pMeshObject, &nVertexCount);
    if (hResult != LIB3MF_OK)
        return hResult;

    // Retrieve Mesh Triangle Count
    hResult = lib3mf_meshobject_gettrianglecount(pMeshObject, &nTriangleCount);
    if (hResult != LIB3MF_OK)
        return hResult;

    // Retrieve Mesh Beam Count
    hResult = lib3mf_meshobject_getbeamcount(pMeshObject, &nBeamCount);
    if (hResult != LIB3MF_OK)
        return hResult;

    if (nBeamCount > 0) {
        hResult = lib3mf_meshobject_getbeamsetcount(pMeshObject, &nBeamSetCount);
        if (hResult != LIB3MF_OK)
            return hResult;
        hResult = lib3mf_meshobject_getbeamlattice_representation(pMeshObject, &bHasRepresentation, &nRepresentationMesh);
        if (hResult != LIB3MF_OK)
            return hResult;
        hResult = lib3mf_meshobject_getbeamlattice_clipping(pMeshObject, &nClipMode, &nClippingMesh);
        if (hResult != LIB3MF_OK)
            return hResult;
    }


    DWORD nSliceStackId;
    hResult = lib3mf_meshobject_getslicestackid(pMeshObject, &nSliceStackId);
    if (hResult != LIB3MF_OK)
        return hResult;

    DWORD nTotalPolygons = 0;
    if (nSliceStackId > 0) {
        PLib3MFSliceStack *pSliceStack;
        hResult = lib3mf_model_getslicestackById(pModel, nSliceStackId, &pSliceStack);
        if (hResult != LIB3MF_OK)
            return hResult;
        hResult = lib3mf_slicestack_getslicecount(pSliceStack, &nSliceCount);
        if (hResult != LIB3MF_OK)
            return hResult;
        for (DWORD iSlice = 0; iSlice < nSliceCount; iSlice++) {
            PLib3MFSlice *pSlice;
            hResult = lib3mf_slicestack_getslice(pSliceStack, iSlice, &pSlice);
            if (hResult != LIB3MF_OK)
                return hResult;
            DWORD nPolygonCount;
            hResult = lib3mf_slice_getpolygoncount(pSlice, &nPolygonCount);
            if (hResult != LIB3MF_OK)
                return hResult;
            for (DWORD polInd = 0; polInd < nPolygonCount; polInd++) {
                DWORD nPolygonIndexCount;
                hResult = lib3mf_slice_getpolygonindexcount(pSlice, polInd, &nPolygonIndexCount);
                if (hResult != LIB3MF_OK)
                    return hResult;
                nTotalPolygons += nPolygonIndexCount;
                std::vector<DWORD> vctPoldIndices(nPolygonIndexCount);
                hResult = lib3mf_slice_getpolygonindices(pSlice, polInd, &vctPoldIndices[0], nPolygonIndexCount);
                if (hResult != LIB3MF_OK)
                    return hResult;
            }
        }
    }

    return LIB3MF_OK;
}

HRESULT ShowComponentsObjectInformation(_In_ PLib3MFModelComponentsObject * pComponentsObject)
{
    HRESULT hResult;
    hResult = ShowObjectProperties(pComponentsObject);
    if (hResult != LIB3MF_OK)
        return hResult;

    // Retrieve Component
    DWORD nComponentCount;
    DWORD nIndex;
    hResult = lib3mf_componentsobject_getcomponentcount(pComponentsObject, &nComponentCount);
    if (hResult != LIB3MF_OK)
        return hResult;


    for (nIndex = 0; nIndex < nComponentCount; nIndex++) {
        DWORD nResourceID;
        PLib3MFModelComponent * pComponent;
        hResult = lib3mf_componentsobject_getcomponent(pComponentsObject, nIndex, &pComponent);
        if (hResult != LIB3MF_OK) {
            return hResult;
        }

        hResult = lib3mf_component_getobjectresourceid(pComponent, &nResourceID);
        if (hResult != LIB3MF_OK) {
            lib3mf_release(pComponent);
            return hResult;
        }

        BOOL bHasTransform;
        hResult = lib3mf_component_hastransform(pComponent, &bHasTransform);
        if (hResult != LIB3MF_OK) {
            lib3mf_release(pComponent);
            return hResult;
        }

        if (bHasTransform) {
            MODELTRANSFORM Transform;

            // Retrieve Transform
            hResult = lib3mf_component_gettransform(pComponent, &Transform);
            if (hResult != LIB3MF_OK) {
                lib3mf_release(pComponent);
                return hResult;
            }

        }

    }

    return LIB3MF_OK;
}


HRESULT ShowMetaDataInformation(_In_ PLib3MFModel * pModel)
{
    HRESULT hResult;
    DWORD nMetaCount;

    hResult = lib3mf_model_getmetadatacount(pModel, &nMetaCount);
    if (hResult != LIB3MF_OK) {
        return -1;
    }

    for (DWORD iMeta = 0; iMeta < nMetaCount; iMeta++) {
        std::string sMetaDataKey;
        std::string sMetaDataValue;
        DWORD nNeededChars;
        hResult = lib3mf_model_getmetadatakeyutf8(pModel, iMeta, NULL, 0, &nNeededChars);
        if (hResult != LIB3MF_OK) {
            return hResult;
        }
        // Retrieve Mesh Name
        if (nNeededChars > 0) {
            std::vector<char> pBuffer;
            pBuffer.resize(nNeededChars + 1);
            hResult = lib3mf_model_getmetadatakeyutf8(pModel, iMeta, &pBuffer[0], nNeededChars + 1, NULL);
            if (hResult != LIB3MF_OK) {
                return hResult;
            }
            pBuffer[nNeededChars] = 0;
            sMetaDataKey = std::string(&pBuffer[0]);
        }

        hResult = lib3mf_model_getmetadatavalueutf8(pModel, iMeta, NULL, 0, &nNeededChars);
        if (hResult != LIB3MF_OK) {
            return hResult;
        }
        // Retrieve Mesh Name
        if (nNeededChars > 0) {
            std::vector<char> pBuffer;
            pBuffer.resize(nNeededChars + 1);
            hResult = lib3mf_model_getmetadatavalueutf8(pModel, iMeta, &pBuffer[0], nNeededChars + 1, NULL);
            if (hResult != LIB3MF_OK) {
                return hResult;
            }
            pBuffer[nNeededChars] = 0;
            sMetaDataValue = std::string(&pBuffer[0]);
        }
    }
    return LIB3MF_OK;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    // General Variables
    HRESULT hResult;
    DWORD nInterfaceVersionMajor, nInterfaceVersionMinor, nInterfaceVersionMicro;
    BOOL pbHasNext;
    DWORD nWarningCount, nErrorCode, nNeededChars;
    LPCSTR pszErrorMessage;

    // Objects
    PLib3MFModel * pModel;
    PLib3MFModelReader * p3MFReader;
    PLib3MFModelBuildItemIterator * pBuildItemIterator;
    PLib3MFModelResourceIterator * pResourceIterator;



    // Check 3MF Library Version
    hResult = lib3mf_getinterfaceversion(&nInterfaceVersionMajor, &nInterfaceVersionMinor, &nInterfaceVersionMicro);
    if (hResult != LIB3MF_OK) {
        return -1;
    }

    if ((nInterfaceVersionMajor != NMR_APIVERSION_INTERFACE_MAJOR)) {
        return -1;
    }
    if (!(nInterfaceVersionMinor >= NMR_APIVERSION_INTERFACE_MINOR)) {
        return -1;
    }

    // Create Model Instance
    hResult = lib3mf_createmodel(&pModel);
    if (hResult != LIB3MF_OK) {
        return -1;
    }

    // Create Model Reader
    hResult = lib3mf_model_queryreader(pModel, "3mf", &p3MFReader);
    if (hResult != LIB3MF_OK) {
        lib3mf_getlasterror(pModel, &nErrorCode, &pszErrorMessage);
        lib3mf_release(pModel);
        return -1;
    }

    // And deactivate the strict mode (default is "false", anyway. This just demonstrates where/how to use it).
    hResult = lib3mf_reader_setstrictmodeactive(p3MFReader, false);

    // Import Model from 3MF File
    bool bErrorOnRead = false;
    hResult = lib3mf_reader_readfrombuffer(p3MFReader, (BYTE*) Data, Size);
    if (hResult != LIB3MF_OK) {
        lib3mf_getlasterror(p3MFReader, &nErrorCode, &pszErrorMessage);
        // don't abort right away
        bErrorOnRead = true;
    }

    // Check warnings in any case:
    hResult = lib3mf_reader_getwarningcount(p3MFReader, &nWarningCount);
    if (hResult != LIB3MF_OK) {
        lib3mf_release(p3MFReader);
        lib3mf_release(pModel);
        return -1;
    }

    for (DWORD iWarning = 0; iWarning < nWarningCount; iWarning++) {
        hResult = lib3mf_reader_getwarningutf8(p3MFReader, iWarning, &nErrorCode, NULL, 0, &nNeededChars);
        if (hResult != LIB3MF_OK) {
            lib3mf_release(p3MFReader);
            lib3mf_release(pModel);
            return hResult;
        }

        std::string sWarning;
        sWarning.resize(nNeededChars + 1);
        hResult = lib3mf_reader_getwarningutf8(p3MFReader, iWarning, &nErrorCode, &sWarning[0], nNeededChars + 1, NULL);
        if (hResult != LIB3MF_OK) {
            lib3mf_release(p3MFReader);
            lib3mf_release(pModel);
            return hResult;
        }
        // Insert custom warning handling here
    }

    // Finally stop if we had an error on read
    if (bErrorOnRead) {
        lib3mf_release(p3MFReader);
        lib3mf_release(pModel);
        return -1;
    }

    // Release model reader
    lib3mf_release(p3MFReader);


    // get metadata
    hResult = ShowMetaDataInformation(pModel);
    if (hResult != LIB3MF_OK) {
        lib3mf_release(pModel);
        return -1;
    }

    // Iterate through all the Objects
    hResult = lib3mf_model_getobjects(pModel, &pResourceIterator);
    if (hResult != LIB3MF_OK) {
        lib3mf_release(pModel);
        return -1;
    }

    hResult = lib3mf_resourceiterator_movenext(pResourceIterator, &pbHasNext);
    if (hResult != LIB3MF_OK) {
        lib3mf_release(pResourceIterator);
        lib3mf_release(pModel);
        return -1;
    }

    while (pbHasNext) {
        PLib3MFModelResource * pResource;
        PLib3MFModelMeshObject * pMeshObject;
        PLib3MFModelComponentsObject * pComponentsObject;
        ModelResourceID ResourceID;

        // get current resource
        hResult = lib3mf_resourceiterator_getcurrent(pResourceIterator, &pResource);
        if (hResult != LIB3MF_OK) {
            lib3mf_release(pResourceIterator);
            lib3mf_release(pModel);
            return -1;
        }

        // get resource ID
        hResult = lib3mf_resource_getresourceid(pResource, &ResourceID);
        if (hResult != LIB3MF_OK) {
            lib3mf_release(pResource);
            lib3mf_release(pResourceIterator);
            lib3mf_release(pModel);
            return -1;
        }

        // Query mesh interface
        BOOL bIsMeshObject;
        hResult = lib3mf_object_ismeshobject(pResource, &bIsMeshObject);
        if ((hResult == LIB3MF_OK) && (bIsMeshObject)) {

            pMeshObject = pResource;

            // Show Mesh Object Information
            hResult = ShowMeshObjectInformation(pMeshObject, pModel);
            if (hResult != LIB3MF_OK) {
                lib3mf_release(pResource);
                lib3mf_release(pResourceIterator);
                lib3mf_release(pModel);
                return -1;
            }
        }


        // Query component interface
        BOOL bIsComponentsObject;
        hResult = lib3mf_object_iscomponentsobject(pResource, &bIsComponentsObject);
        if ((hResult == LIB3MF_OK) && (bIsComponentsObject)) {

            pComponentsObject = (PLib3MFModelComponentsObject*)pResource;

            // Show Component Object Information
            hResult = ShowComponentsObjectInformation(pComponentsObject);
            if (hResult != LIB3MF_OK) {
                lib3mf_release(pResource);
                lib3mf_release(pResourceIterator);
                lib3mf_release(pModel);
                return -1;
            }
        }

        // free instances
        lib3mf_release(pResource);

        hResult = lib3mf_resourceiterator_movenext(pResourceIterator, &pbHasNext);
        if (hResult != LIB3MF_OK) {
            return -1;
        }
    }

    // Release Resource Iterator
    lib3mf_release(pResourceIterator);


    // Iterate through all the Build items
    hResult = lib3mf_model_getbuilditems(pModel, &pBuildItemIterator);
    if (hResult != LIB3MF_OK) {
        lib3mf_release(pBuildItemIterator);
        lib3mf_release(pModel);
        return -1;
    }

    hResult = lib3mf_builditemiterator_movenext(pBuildItemIterator, &pbHasNext);
    if (hResult != LIB3MF_OK) {
        lib3mf_release(pBuildItemIterator);
        lib3mf_release(pModel);
        return -1;
    }

    while (pbHasNext) {

        DWORD ResourceID;
        MODELTRANSFORM Transform;
        PLib3MFModelBuildItem * pBuildItem;
        // Retrieve Build Item
        hResult = lib3mf_builditemiterator_getcurrent(pBuildItemIterator, &pBuildItem);
        if (hResult != LIB3MF_OK) {
            lib3mf_release(pBuildItemIterator);
            lib3mf_release(pModel);
            return -1;
        }

        // Retrieve Resource
        PLib3MFModelObjectResource * pObjectResource;
        hResult = lib3mf_builditem_getobjectresource(pBuildItem, &pObjectResource);
        if (hResult != LIB3MF_OK) {
            lib3mf_release(pBuildItem);
            lib3mf_release(pBuildItemIterator);
            lib3mf_release(pModel);
            return -1;
        }

        // Retrieve Resource ID
        hResult = lib3mf_resource_getresourceid(pObjectResource, &ResourceID);
        if (hResult != LIB3MF_OK) {
            lib3mf_release(pObjectResource);
            lib3mf_release(pBuildItem);
            lib3mf_release(pBuildItemIterator);
            lib3mf_release(pModel);
            return -1;
        }

        // Release Object Resource ID
        lib3mf_release(pObjectResource);


        // Check Object Transform
        BOOL bHasTransform;
        hResult = lib3mf_builditem_hasobjecttransform(pBuildItem, &bHasTransform);
        if (hResult != LIB3MF_OK) {
            lib3mf_release(pBuildItem);
            lib3mf_release(pBuildItemIterator);
            lib3mf_release(pModel);
            return -1;
        }

        if (bHasTransform) {
            // Retrieve Transform
            hResult = lib3mf_builditem_getobjecttransform(pBuildItem, &Transform);
            if (hResult != LIB3MF_OK) {
                lib3mf_release(pBuildItem);
                lib3mf_release(pBuildItemIterator);
                lib3mf_release(pModel);
                return -1;
            }

        }


        // Retrieve Mesh Part Number Length
        std::string sPartNumber;
        DWORD nNeededChars;
        hResult = lib3mf_builditem_getpartnumberutf8(pBuildItem, NULL, 0, &nNeededChars);
        if (hResult != LIB3MF_OK) {
            lib3mf_release(pBuildItem);
            lib3mf_release(pBuildItemIterator);
            lib3mf_release(pModel);
            return hResult;
        }

        // Retrieve Mesh Name
        if (nNeededChars > 0) {
            std::vector<char> pBuffer;
            pBuffer.resize(nNeededChars + 1);
            hResult = lib3mf_builditem_getpartnumberutf8(pBuildItem, &pBuffer[0], nNeededChars + 1, NULL);
            pBuffer[nNeededChars] = 0;
            sPartNumber = std::string(&pBuffer[0]);
        }

        // Release Build Item
        lib3mf_release(pBuildItem);

        // Move to next Item
        hResult = lib3mf_builditemiterator_movenext(pBuildItemIterator, &pbHasNext);
        if (hResult != LIB3MF_OK) {
            return -1;
        }
    }


    // Release Build Item Iterator
    lib3mf_release(pBuildItemIterator);

    // Release Model
    lib3mf_release(pModel);

    return 0;
}