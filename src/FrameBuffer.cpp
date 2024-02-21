#include "FrameBuffer.h"

void FrameBuffer::setUpFrameBuffer() {
    // ����һ��֡����ռ�
    glGenFramebuffers(1, &ID);
    // ��֡����Ϊ��ǰ֡����
    glBindFramebuffer(GL_FRAMEBUFFER, ID);
    
    if (isDepthBuff) {
        // ������֡����ֻ���������������ͼ
        // ������ͼ��ʽ����ΪGL_DEPTH_COMPONENT
        Texture textureDepthMap = Texture::CreateTexture(width, height, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT);
        textureID = textureDepthMap.ID;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        glBindTexture(GL_TEXTURE_2D, 0);

        // ������ȿ���ͼ����ǰ֡���壬���ø�ʽΪGL_DEPTH_ATTACHMENT
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureDepthMap.ID, 0);
        
        // ��Ⱦ���������Ҫ��ֱ����ʽ����OpenGL��д����Ϊ��
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        // ����ǰ֡����󶨻�Ĭ�ϻ���
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return;
    }
    
    // ���ɸ�����ͼ��������֡����Ľ��
    Texture textureColorMap = Texture::CreateTexture(width, height);
    textureID = textureColorMap.ID;

    // ������ͼ���Ÿ���ǰ֡����
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorMap.ID, 0);
    
    // ����һ����Ⱦ�������
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    // ����Ⱦ�������Ϊ��ǰ��Ⱦ�������
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    // ʹ��һ����Ⱦ�������䵱��Ȼ����ģ�建�壬ǰ24bit����Ȼ��壬��8bit��ģ�建��
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); 
    renderBufferID = rbo;

    // ����Ⱦ��������Ÿ���ǰ֡����
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); 
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "֡�������ɲ�������" << std::endl;

    // ����ǰ֡����󶨻�Ĭ�ϻ���
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
