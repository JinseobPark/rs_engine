#ifndef VTK_LOADER_DATA_H_
#define VTK_LOADER_DATA_H_

namespace SJG
{
  class VTKLoaderData
  {
  public:
    VTKLoaderData();
    ~VTKLoaderData();

    void Initialize();
    void Shutdown();

    void Update(float dt);

    void KeyUpdate(float dt);

    void LoadVTKFile(const char* file_path);

  private:
    RS_Widget::RSLegendWidget* m_legend_widget = nullptr;
  };
}

#endif // !VTK_LOADER_DATA_H_
