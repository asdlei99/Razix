#pragma once
#include "RZEvent.h"

namespace Razix {

	class RAZIX_API RZWindowResizeEvent : public RZEvent
	{
	public:
		RZWindowResizeEvent(unsigned int width, unsigned int height)
			: m_Width(width), m_Height(height) {}

		inline unsigned int GetWidth() const { return m_Width; }
		inline unsigned int GetHeight() const { return m_Height; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: (" << m_Width << ", " << m_Height << ")";
			return ss.str();
		}

//		EVENT_CLASS_TYPE(EventType::WindowResize)
        static EventType GetStaticType() { return EventType::WindowResize; }
        virtual EventType GetEventType() const override { return GetStaticType(); }
        virtual const char* GetName() const override { return "test"; }
		EVENT_CLASS_CATEGORY((int)EventCategory::EventCategoryApplication)
	private:
		unsigned int m_Width, m_Height;
	};

	class RAZIX_API WindowCloseEvent : public RZEvent
	{
	public:
		WindowCloseEvent() {}

//		EVENT_CLASS_TYPE(return EventType::WindowClose)
        static EventType GetStaticType() { return EventType::WindowClose; }
        virtual EventType GetEventType() const override { return GetStaticType(); }
        virtual const char* GetName() const override { return "test"; }
		EVENT_CLASS_CATEGORY((int)EventCategory::EventCategoryApplication)
	};

	class RAZIX_API AppTickEvent : public RZEvent
	{
	public:
		AppTickEvent() {}

//		EVENT_CLASS_TYPE(return EventType::AppTick)
        static EventType GetStaticType() { return EventType::AppTick; }
        virtual EventType GetEventType() const override { return GetStaticType(); }
        virtual const char* GetName() const override { return "test"; }
		EVENT_CLASS_CATEGORY((int)EventCategory::EventCategoryApplication)
	};

	class RAZIX_API AppUpdateEvent : public RZEvent
	{
	public:
		AppUpdateEvent() {}

//		EVENT_CLASS_TYPE(return EventType::AppUpdate)
        static EventType GetStaticType() { return EventType::AppUpdate; }
        virtual EventType GetEventType() const override { return GetStaticType(); }
        virtual const char* GetName() const override { return "test"; }
		EVENT_CLASS_CATEGORY((int)EventCategory::EventCategoryApplication)
	};

	class RAZIX_API AppRenderEvent : public RZEvent
	{
	public:
		AppRenderEvent() {}

//		EVENT_CLASS_TYPE(return EventType::AppRender)
        static EventType GetStaticType() { return EventType::AppRender; }
        virtual EventType GetEventType() const override { return GetStaticType(); }
        virtual const char* GetName() const override { return "test"; }
		EVENT_CLASS_CATEGORY((int)EventCategory::EventCategoryApplication)
	};
}
