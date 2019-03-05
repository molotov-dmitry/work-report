#!/bin/bash

ROOT_PATH="$(cd "$(dirname "$0")" && pwd)"
cd "${ROOT_PATH}" || exit 1

mkdir -p /usr/local/bin
cp -f work-report /usr/local/bin/

mkdir -p "/usr/share/applications/"

cat << EOF > "/usr/share/applications/work-report.desktop"
[Desktop Entry]
Version=1.0

Name=Report Master
Name[ru]=Мастер отчетов

Comment=Make a beautiful report and send it to your supervisor
Comment[ru]=Создай шикарный отчёт и отправь его своему руководителю

GenericName=Work report editor
GenericName[ru]=Редактор отчетов

Keywords=Report
Keywords[ru]=Отчет;Отчёт

Exec=work-report
Terminal=false
X-MultipleArgs=false
Type=Application
Icon=time-admin
Categories=GNOME;GTK;Network;WebBrowser;
StartupNotify=true
EOF
