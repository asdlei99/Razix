#pragma once

#include "generated/ui_RZEWorldSettingsWindow.h"
#include <QWidget>

namespace Razix {
    namespace Editor {

        class RZEWorldSettingsWindow : public QWidget
        {
            Q_OBJECT

        public:
            RZEWorldSettingsWindow(QWidget *parent = nullptr);
            ~RZEWorldSettingsWindow();

        public slots:
            // Environment Settings
            void On_UseProceduralSkybox();

            // Bloom Settings
            void On_SetFilterRadius();
            void On_SetStrength();

        private:
            Ui::WorldSettings ui;
        };
    }    // namespace Editor
}    // namespace Razix