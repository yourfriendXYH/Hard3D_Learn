// drawable的矩阵变化的常数缓存
cbuffer CBuf
{
    matrix modelView; // 世界矩阵 * 相机矩阵
    matrix modelViewProj; // 世界矩阵 * 相机矩阵 * 投影矩阵
};
