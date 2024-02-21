#include "FrameBuffer.h"

void FrameBuffer::setUpFrameBuffer() {
    // 开辟一个帧缓冲空间
    glGenFramebuffers(1, &ID);
    // 绑定帧缓冲为当前帧缓冲
    glBindFramebuffer(GL_FRAMEBUFFER, ID);
    
    if (isDepthBuff) {
        // 如果这个帧缓冲只想用于生成深度贴图
        // 将空贴图格式设置为GL_DEPTH_COMPONENT
        Texture textureDepthMap = Texture::CreateTexture(width, height, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT);
        textureID = textureDepthMap.ID;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        glBindTexture(GL_TEXTURE_2D, 0);

        // 附着深度空贴图给当前帧缓冲，设置格式为GL_DEPTH_ATTACHMENT
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureDepthMap.ID, 0);
        
        // 渲染缓冲对象不需要，直接显式告诉OpenGL读写缓冲为空
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        // 将当前帧缓冲绑定回默认缓冲
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return;
    }
    
    // 生成个空贴图用来接收帧缓冲的结果
    Texture textureColorMap = Texture::CreateTexture(width, height);
    textureID = textureColorMap.ID;

    // 将空贴图附着给当前帧缓冲
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorMap.ID, 0);
    
    // 开辟一个渲染缓冲对象
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    // 绑定渲染缓冲对象为当前渲染缓冲对象
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    // 使用一个渲染缓存对象充当深度缓冲和模板缓冲，前24bit是深度缓冲，后8bit是模板缓冲
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); 
    renderBufferID = rbo;

    // 将渲染缓冲对象附着给当前帧缓冲
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); 
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "帧缓冲生成不完整！" << std::endl;

    // 将当前帧缓冲绑定回默认缓冲
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
