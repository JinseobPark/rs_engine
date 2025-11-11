/******************************************************************************
 * \file   PointClipperData.h
 * \brief  Point Clipper Data
 * 
 * \author Jinseob Park
 * \date   06/11/2024
 *****************************************************************************/
#ifndef POINT_CLIPPER_DATA_H_
#define POINT_CLIPPER_DATA_H_


namespace SJG
{
  class PointClipperData
  {
  public:
    PointClipperData();
    ~PointClipperData();

    void Initialize();
    void Shutdown();

    void Update(float dt);

    void KeyUpdate(float dt);

    void LoadBinFile(const char* file_path);

  private:
  };
}


#endif // !POINT_CLIPPER_DATA_H_
