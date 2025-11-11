/*****************************************************************//**
 * \file   RSFBO.h
 * \brief  RS engine Frame buffer object class for rendering system.
 * 
 * \author Jinseob Park
 * \date   March 2025
 *********************************************************************/
#ifndef RS_FBO_H_
#define RS_FBO_H_


/**
  * @brief Rendering System Internal namespace
  * This namespace is for internal use only.
 */
namespace _RS_Internal
{
  /**
 * @brief Frame buffer object base class
 */
class RSFBO
{
public:
  RSFBO() = default;
  virtual ~RSFBO() = default;

  /**
   * @brief Create Frame buffer object
   */
  virtual void CreateFbo(const int width_, const int height_) = 0;

  /**
   * @brief Bind Frame buffer object
   */
  virtual void BindFbo() const = 0;

  /**
   * @brief Unbind Frame buffer object
   */
  void UnbindFbo();


  /**
   * @brief Resize Frame buffer object and textures (virtual function)
   * @param width_ texture width
   * @param height_ texture height
   */
  virtual void Resize(int width_, int height_) = 0;

  /**
   * @brief Release buffers (Delete buffers)
   */
  virtual void ReleaseFbo() = 0;

protected:
  /**
   * @brief Check frame buffer status
   * @return true if frame buffer is complete
   */
  bool CheckFrameBufferStatus();

};

} // namespace _RS_Internal

#endif // !_RSFBO_H_
