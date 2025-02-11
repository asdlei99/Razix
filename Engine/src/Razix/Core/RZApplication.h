#pragma once

#include "Razix/Core/OS/RZWindow.h"
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZProfiling.h"
#include "Razix/Core/RZSmartPointers.h"
#include "Razix/Core/RZUUID.h"

#include "Razix/Events/ApplicationEvent.h"
#include "Razix/Events/RZKeyEvent.h"
#include "Razix/Events/RZMouseEvent.h"

#include "Razix/Scene/RZEntity.h"

#include "Razix/Utilities/RZTimer.h"
#include "Razix/Utilities/RZTimestep.h"
#include "Razix/Utilities/TRZSingleton.h"

#include "Razix/Graphics/RHI/API/RZSwapchain.h"

#include "Razix/Graphics/RHI/RZGPUProfiler.h"

// glm
#include <glm/glm.hpp>

// Cereal
#pragma warning(push, 0)
#include <cereal/types/vector.hpp>
#pragma warning(pop)

//! Some style guide rules are waved off for RZApplication class

namespace ImGuizmo {
    enum OPERATION;
    enum MODE;
}    // namespace ImGuizmo

namespace Razix::Graphics {
    class RZTexture;
}    // namespace Razix::Graphics

namespace Razix {

    /* Determines the state of the application */
    enum class AppState
    {
        Running,
        Loading,
        Closing
    };

    /* The type of the application (Editor or Game) */
    enum class AppType
    {
        EDITOR,
        GAME
    };

    /* Creates an Razix Application (Used as the base for Editor, Sandbox and Game Project) */
    class RAZIX_API RZApplication : public RZRoot
    {
    public:
        static std::mutex              m;
        static std::condition_variable halt_execution;
        static bool                    ready_for_execution;

    public:
        /**
         * Creates a Razix Application
         * 
         * @param projectRoot   The root location of the application
         * @param appName       The name of the Razix application
         */
        RAZIX_CALL RZApplication(const std::string& projectRoot, const std::string& appName = "Razix App");
        /* Simple Virtual destructor */
        virtual ~RZApplication() {}

        /* Gets the static reference to the application instance */
        inline static RZApplication& RAZIX_CALL Get() { return *s_AppInstance; }

        // TODO: Have 2 inits ==> Static and Runtime
        /* Initializes the application and other runtime systems */
        void RAZIX_CALL Init();

        /* Starts the Engine Runtime systems */
        void RAZIX_CALL Run();

        /* Quits the application and releases any resources held by it */
        void Quit();

        /* Save the application */
        void SaveApp();

        /**
         * Renders the frame and displays the graphics and updates the window
         * 
         * @returns True if the frame was successfully rendered by checking the state
         */
        bool RenderFrame();

        /**
         * Called before the application starts rendering
         * This is called after the application and all the Engine systems are Initialized and just before OnRender() is called
         */
        virtual void RAZIX_CALL OnStart() {}
        /**
         * Updates the Engine systems for every engine timestep
         * 
         * @param dt The timestep taken for every frame
         */
        virtual void RAZIX_CALL OnUpdate(const RZTimestep& dt) {}
        /**
         * Calls the engine sub-systems to render the stuff calculated in OnFrame()
         * Begins the frame and submits the rendergraph to final display
         */
        virtual void RAZIX_CALL OnRender() {}
        /**
         * Called before the application is quit
         */
        virtual void RAZIX_CALL OnQuit() {}
        /**
         * Called for Rendering ImGui UI
         */
        virtual void RAZIX_CALL OnImGui() {}

        // Event callbacks for client
        virtual void RAZIX_CALL OnResize(u32 width, u32 height) {}

        /* Returns a reference to the application window */
        inline RZWindow* RAZIX_CALL getWindow() { return m_Window; }
        /* Gets the window size */
        inline glm::vec2 RAZIX_CALL getWindowSize() { return glm::vec2(m_Window->getWidth(), m_Window->getHeight()); }
        /* Returns a reference to the Application instance */
        inline std::string RAZIX_CALL getAppName() const { return m_ProjectName; }
        /* Gets the razixproject file path */
        inline std::string getAppFilePath() const { return m_ProjectFilePath; }
        /* Gets the window properties */
        inline WindowProperties& RAZIX_CALL getWindowProps() { return m_WindowProperties; }
        /* Gets the application render loop timer */
        inline RZTimer RAZIX_CALL getTimer() { return *m_Timer.get(); }

        void setViewportWindow(RZWindow* viewportWindow) { m_Window = viewportWindow; }
        void setViewportHWND(HWND hwnd) { viewportHWND = hwnd; }
        HWND getViewportHWND() { return viewportHWND; }

        void setProjectRoot(const std::string& projPath) { m_ProjectFilePath = projPath; }

        inline AppType getAppType() { return m_appType; }
        void           setAppType(AppType appType) { m_appType = appType; }

        // Guizmo Operations
        void disableGuizmoEditing() { m_EnableGuizmoEditing = false; }
        void setGuizmoForEntity(RZEntity& entity)
        {
            m_GuizmoEntity        = entity;
            m_EnableGuizmoEditing = true;
        }
        void setGuizmoOperation(ImGuizmo::OPERATION operation) { m_GuizmoOperation = operation; }
        void setGuizmoMode(ImGuizmo::MODE mode) { m_GuizmoMode = mode; }
        void setGuizmoSnapAmount(f32 snapAmount) { m_GuizmoSnapAmount = snapAmount; }

        RAZIX_INLINE const AppState& getAppState() const { return m_CurrentState; }
        void                         setAppState(AppState state) { m_CurrentState = state; }

        // Application Save and Load Functions
        RAZIX_DEFINE_SAVE_LOAD

    private:
        HWND                      viewportHWND;
        static RZApplication*     s_AppInstance;                      /* The singleton instance of the application                */
        AppState                  m_CurrentState = AppState::Loading; /* The current state of the application                     */
        AppType                   m_appType      = AppType::GAME;     /* The type of the application                              */
        std::string               m_ProjectName;                      /* The name of the application                              */
        std::string               m_ProjectFilePath;                  /* The path of the Razix Project file (*.razixproject)      */
        std::string               m_ProjectPath;                      /* The path of the Razix Project Assets folder              */
        u32                       m_RenderAPI;                        /* The Render API being used to render the application      */
        u32                       m_Frames  = 0;                      /* The number of frames per second                          */
        u32                       m_Updates = 0;                      /* The number of updated per second                         */
        rzstl::UniqueRef<RZTimer> m_Timer;                            /* The timer used to calculate the delta time and timesteps */
        f32                       m_SecondTimer = 0;                  /* A secondary timer to count the ticks per second          */
        RZTimestep                m_Timestep;                         /* The timesteps taken to update the application            */
        RZWindow*                 m_Window = nullptr;                 /* The window that will be used to view graphics            */
        WindowProperties          m_WindowProperties;                 /* The properties of the window to create with              */
        RZUUID                    m_ProjectID;                        /* Project ID is a UUID to uniquely identify project        */
        std::vector<std::string>  sceneFilePaths;
        RZEntity                  m_GuizmoEntity;
        ImGuizmo::OPERATION       m_GuizmoOperation;
        ImGuizmo::MODE            m_GuizmoMode;
        f32                       m_GuizmoSnapAmount = 0.0f;
        Graphics::RZGPUProfiler   m_GPUProfiler;
        bool                      m_EnableGuizmoEditing = false;

    private:
        /* Starts the application */
        void Start();
        /* Updates the engine and application runtime systems */
        void Update(const RZTimestep& dt);
        /* Renders the application and Engine rendering commands */
        void Render();
        /* Used to render GUI */
        void RenderGUI();

        // Event callbacks
        /**
         * Gets the Events from the engine, window and OS
         * 
         * @param event  The event received from all the sub-systems
         */
        void OnEvent(RZEvent& event);

        /**
         * Called when the application is about to be closed
         *
         * @param e The window close event
         *
         * @returns True, if the window was closed successfully
         */
        bool OnWindowClose(WindowCloseEvent& e);
        /**
         * Called when the window is resized
         *
         * @param e The window resize event
         *
         * @returns True, if the window was resized successfully
         */
        virtual bool OnWindowResize(RZWindowResizeEvent& e);

        virtual bool OnMouseMoved(RZMouseMovedEvent& e);
        virtual bool OnMouseButtonPressed(RZMouseButtonPressedEvent& e);
        virtual bool OnMouseButtonReleased(RZMouseButtonReleasedEvent& e);
        virtual bool OnKeyPress(RZKeyPressedEvent& e);
        virtual bool OnKeyRelease(RZKeyReleasedEvent& e);

        RAZIX_NONCOPYABLE_CLASS(RZApplication);
    };

    /**
     * To be defined on the CLIENT side to create the application
     *
     * What this means is that the Entry point thinks the engine will define this for sure.
     * In fact it does by just forward declaring, but who actually defines it?
     * Now the engine forces the client to implement this according to their needs. 
     * [Application(forward declaration)-->Entry Point(extern declaration)-->CLIENT(definition)]
     * Defined by the client to create the application definition
     */
    RZApplication* CreateApplication(int argc, char** argv);
}    // namespace Razix
