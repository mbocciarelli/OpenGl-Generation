#pragma once
#include <memory>
#include <string>
#include <gl/glew.h>

class Texture
{
public:
	virtual ~Texture() = default;

	virtual uint32_t GetWidth() const = 0;
	virtual uint32_t GetHeight() const = 0;
	virtual uint32_t GetRendererID() const = 0;

	virtual const std::string& GetPath() const = 0;

	virtual void SetData(void* data, uint32_t size) = 0;

	virtual void Bind(uint32_t slot = 0) const = 0;

	virtual bool IsLoaded() const = 0;

	virtual bool operator==(const Texture& other) const = 0;
};

class Texture2D : public Texture
{
public:
	Texture2D(uint32_t width, uint32_t height);
	Texture2D(const std::string& path);
	~Texture2D() override;

	uint32_t GetWidth() const override { return m_Width; }
	uint32_t GetHeight() const override { return m_Height; }
	uint32_t GetRendererID() const override { return m_RendererID; }

	const std::string& GetPath() const override { return m_Path; }

	void SetData(void* data, uint32_t size) override;

	void Bind(uint32_t slot = 0) const override;

	bool IsLoaded() const override { return m_IsLoaded; }

	bool operator==(const Texture& other) const override
	{
		return m_RendererID == other.GetRendererID();
	}

	static std::shared_ptr<Texture2D> Create(uint32_t width, uint32_t height)
	{
		return std::make_shared<Texture2D>(width, height);
	}
	static std::shared_ptr<Texture2D> Create(const std::string& path)
	{
		return std::make_shared<Texture2D>(path);
	}

private:
	std::string m_Path;
	bool m_IsLoaded = false;
	uint32_t m_Width, m_Height;
	uint32_t m_RendererID;
	GLenum m_InternalFormat, m_DataFormat;
};