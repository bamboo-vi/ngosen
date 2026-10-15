# SPDX-FileCopyrightText: 2026 Ngó Sen contributors
#
# SPDX-License-Identifier: GPL-3.0-or-later
"""Shows what is wrong with this machine's fcitx5 setup and opens a pre-filled bug report."""

from i18n import _
from qtpy.QtCore import QTimer, QUrl
from qtpy.QtGui import QDesktopServices, QGuiApplication
from qtpy.QtWidgets import (
    QDialog,
    QDialogButtonBox,
    QHBoxLayout,
    QLabel,
    QStyle,
    QVBoxLayout,
    QWidget,
)

from core import system_check
from ui.typography import mute

ICONS = {
    system_check.OK: QStyle.SP_DialogApplyButton,
    system_check.WARN: QStyle.SP_MessageBoxWarning,
    system_check.FAIL: QStyle.SP_MessageBoxCritical,
}


class SystemCheckDialog(QDialog):
    def __init__(self, dbus_handler, ngosen_version="", parent=None):
        super().__init__(parent)
        self.dbus_handler = dbus_handler
        self.ngosen_version = ngosen_version
        self.setWindowTitle(_("Check my machine"))
        self.setMinimumWidth(520)

        layout = QVBoxLayout(self)
        intro = QLabel(
            _(
                "How fcitx5 is set up on this machine. Nothing is sent until you send a bug "
                "report, and it lists the apps below, so read it before sending."
            )
        )
        intro.setWordWrap(True)
        mute(intro)
        layout.addWidget(intro)

        self.results = QVBoxLayout()
        self.results.setSpacing(12)
        layout.addLayout(self.results)

        self.apps = QLabel()
        self.apps.setObjectName("CheckApps")
        self.apps.setWordWrap(True)
        mute(self.apps)
        layout.addWidget(self.apps)
        layout.addStretch()

        buttons = QDialogButtonBox(QDialogButtonBox.Close)
        self.again_btn = buttons.addButton(_("Check again"), QDialogButtonBox.ActionRole)
        self.copy_btn = buttons.addButton(_("Copy result"), QDialogButtonBox.ActionRole)
        self.report_btn = buttons.addButton(_("Report bug…"), QDialogButtonBox.ActionRole)
        self.again_btn.clicked.connect(self.run)
        self.copy_btn.clicked.connect(self.copy_report)
        self.report_btn.clicked.connect(self.open_report)
        buttons.rejected.connect(self.reject)
        layout.addWidget(buttons)

        self.run()

    def run(self):
        self.facts = system_check.collect(self.dbus_handler, self.ngosen_version)
        self.findings = system_check.evaluate(self.facts)
        self._show_findings()
        if self.facts.apps:
            names = ", ".join(f"{p} ({fe})" for p, fe in self.facts.apps)
            self.apps.setText(_("Apps connected to fcitx5: {}").format(names))
        else:
            self.apps.setText("")
        # The layout picks up the new rows only on the next event loop pass; resizing now would
        # use the old height and clip the rows.
        QTimer.singleShot(0, self.adjustSize)

    def _show_findings(self):
        while self.results.count():
            item = self.results.takeAt(0)
            row = item.widget()
            if row:
                # Detach now: deleteLater alone leaves the old row painted under the new one.
                row.setParent(None)
                row.deleteLater()
        findings = self.findings or [
            system_check.Finding(system_check.OK, _("No setup problem found"))
        ]
        for finding in findings:
            self.results.addWidget(self._row(finding))

    def _row(self, finding):
        row = QWidget()
        row.setObjectName(f"CheckRow-{finding.level}")
        box = QHBoxLayout(row)
        box.setContentsMargins(0, 0, 0, 0)
        icon = QLabel()
        size = self.style().pixelMetric(QStyle.PM_SmallIconSize)
        icon.setPixmap(self.style().standardIcon(ICONS[finding.level]).pixmap(size, size))
        box.addWidget(icon, 0)
        text = QVBoxLayout()
        text.setSpacing(2)
        text.addWidget(QLabel(finding.title))
        if finding.fix:
            text.addWidget(mute(QLabel(finding.fix)))
        box.addLayout(text, 1)
        return row

    def report_text(self):
        return system_check.report(self.facts, self.findings)

    def copy_report(self):
        QGuiApplication.clipboard().setText(self.report_text())

    def open_report(self):
        QDesktopServices.openUrl(QUrl(system_check.issue_url(self.facts, self.report_text())))
