target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        asset.cppm
        asset_c.ixx
        asset_id_from_string.ixx
        AssetID.ixx
        AssetManager.ixx
        AssetManagerBuilder.ixx
        AssetPool.ixx
        SharedAssetHandle.ixx
        WeakAssetHandle.ixx
)
