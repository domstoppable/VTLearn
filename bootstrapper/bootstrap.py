import os
import time
import shutil

import platform
import subprocess
import pkg_resources

from zipfile import ZipFile
from multiprocessing import Process, freeze_support
from pathlib import Path

from distutils.dir_util import copy_tree

from requests_futures.sessions import FuturesSession

from PySide2.QtWidgets import *
from PySide2.QtGui import *
from PySide2.QtCore import *
from PySide2.QtNetwork import *

imageName = 'vtlearn'
if platform.system().lower() == 'windows':
	imageName += '.exe'

def findAsset(*resourceParts):
	resource = '/'.join(['assets'] + list(resourceParts))
	return pkg_resources.resource_filename(__name__, resource)

def apply_update(filename):

	with ZipFile(filename, 'r') as inputFile:
		inputFile.extractall('updates/')

	unzipDir = Path('updates')/filename.stem
	copy_tree(unzipDir, '.')
	
	filename.unlink()
	shutil.rmtree(unzipDir)
	unzipDir.touch()

def centerWindow(windowWidget):
	windowWidget.setGeometry(
		QStyle.alignedRect(
			Qt.LeftToRight,
			Qt.AlignCenter,
			windowWidget.size(),
			QGuiApplication.primaryScreen().availableGeometry(),
		)
	)

class WobbleImage(QWidget):
	def __init__(self, pixmap, *args, **kwargs):
		super().__init__(*args, **kwargs)

		self._angle = 0

		self.pixmap = QPixmap(pixmap)
		self.setMinimumSize(self.pixmap.size())

		self.animation = QPropertyAnimation(self, b'angle')
		self.animation.setDuration(1000)
		self.animation.setKeyValueAt(0, 0)
		self.animation.setKeyValueAt(.25, 3)
		self.animation.setKeyValueAt(.75, -3)
		self.animation.setKeyValueAt(1, 0)
		self.animation.setLoopCount(-1)

	def setPixmap(self, pixmap):
		self.pixmap = QPixmap(pixmap)
		self.update()

	def startWobble(self):
		self.animation.start()

	def stopWobble(self):
		self.animation.setLoopCount(1)

	@Property(float)
	def angle(self):
		return self._angle

	@angle.setter
	def angle(self, angle):
		self._angle = angle
		self.update()

	def paintEvent(self, event):
		painter = QPainter(self)
		painter.setRenderHint(painter.Antialiasing, True)
		rotationCenter = QPoint(self.width()/2, self.height()*.75)
		painter.translate(rotationCenter)
		painter.rotate(self._angle)
		painter.translate(-rotationCenter)
		painter.drawPixmap(
			(self.width()-self.pixmap.width())/2,
			(self.height()-self.pixmap.height())/2,
			self.pixmap
		)

class BootstrapWindow(QDialog):
	launchButtonPressed = Signal()

	def __init__(self, *args, **kwargs):
		super().__init__(*args, **kwargs)

		self.setWindowFlag(Qt.WindowContextHelpButtonHint, False)
		self.setWindowFlag(Qt.MSWindowsFixedSizeDialogHint, True)

		self.setLayout(QVBoxLayout())

		self.statusLabel = QLabel(self)
		self.statusLabel.setAlignment(Qt.AlignCenter)
		
		self.progressBar = QProgressBar(self)
		self.progressBar.setRange(0, 0)
		self.progressBar.setTextVisible(False)

		self.logo = WobbleImage(findAsset('logo.png'), parent=self)
		self.layout().addWidget(self.logo)
		self.layout().addWidget(self.statusLabel)
		self.layout().addWidget(self.progressBar)

	def resizeEvent(self, event):
		centerWindow(self)

	def setStatus(self, status):
		self.statusLabel.setText(status)

	def setError(self, message):
		self.statusLabel.setText(message)
		self.logo.stopWobble()
		self.logo.setPixmap(findAsset('error.png'))
		self.progressBar.hide()
		self.showLaunchButton()

	def showLaunchButton(self):
		self.launchButton = QPushButton('Launch Training App Anyway', self)
		self.launchButton.pressed.connect(self.onLaunchButtonPressed)
		self.layout().addWidget(self.launchButton)

	def onLaunchButtonPressed(self):
		self.launchButton.setText('Please wait...')
		self.setDisabled(True)
		self.launchButtonPressed.emit()

	def setProgressMax(self, maximum):
		self.progressBar.setMaximum(maximum)

	def setProgress(self, value):
		self.progressBar.setValue(value)

	def setProgressDone(self):
		self.progressBar.setMaximum(1)
		self.progressBar.setValue(1)
		self.logo.stopWobble()

	def addProgress(self, value):
		self.progressBar.setValue(self.progressBar.value() + value)
		self.logo.startWobble()

class KillProcessWindow(QWidget):
	killRequested = Signal()

	def __init__(self, *args, **kwargs):
		super().__init__(*args, **kwargs)
		
		self.setLayout(QVBoxLayout(self))

		self.label = QLabel(self)
		self.label.setAlignment(Qt.AlignCenter)
		self.label.setText('The training app appears to be running already.\n\nWhat would you like to do?')

		buttons = QHBoxLayout()
		button = QPushButton('Cancel')
		button.pressed.connect(self.close)
		buttons.addWidget(button)

		button = QPushButton('Kill the app')
		button.pressed.connect(self.killRequested.emit)
		buttons.addWidget(button)

		self.layout().addWidget(self.label)
		self.layout().addLayout(buttons)

	def resizeEvent(self, event):
		centerWindow(self)
		
	def setStatus(self, status):
		self.label.setText(self.label.text() + '\n\n' + status)

	def sizeHint(self):
		return QSize(300, 200)

class Bootstrapper(QApplication):
	def __init__(self, *args, **kwargs):
		super().__init__(*args, **kwargs)

		self.setApplicationName('VTLearn Update Checker')
		self.setApplicationDisplayName('VTLearn Update Checker')
		self.setWindowIcon(QIcon(QPixmap(findAsset('logo.png'))))
		self.setStyleSheet('''
			QLabel, QAbstractButton { font-size: 12pt; padding: 20px }
			QAbstractButton { font-weight: bold; }
		''')

		self.window = BootstrapWindow()
		self.window.launchButtonPressed.connect(self.launchImmediately)

		self.downloader = QNetworkAccessManager(self)
		self.downloadFileHandle = None

	def exec_(self):
		self.checkForExistingProc()
		super().exec_()

	def checkForExistingProc(self):
		proc = subprocess.run(['qprocess', imageName])
		if proc.returncode == 0:
			self.killWindow = KillProcessWindow()
			self.killWindow.killRequested.connect(self.onKillRequested)
			self.killWindow.show()
			self.window.hide()
		else:
			self.window.show()
			QTimer.singleShot(1, self.checkForUpdate)

	def onKillRequested(self):
		self.killWindow.hide()
		subprocess.run(['Taskkill', '/IM', imageName, '/F'])
		QTimer.singleShot(1000, self.checkForExistingProc)

	def checkForUpdate(self):
		self.window.setStatus('Checking for update...')
		self.versionCheckRequest = QNetworkRequest()
		self.versionCheckRequest.setUrl(QUrl('http://www.greenlightgo.org/vtlearn/LATEST_VERSION'))

		self.versionCheckResponse = self.downloader.get(self.versionCheckRequest)
		self.versionCheckResponse.finished.connect(self.onVersionCheckDownloadComplete)

	def onVersionCheckDownloadComplete(self):
		code = self.versionCheckResponse.attribute(QNetworkRequest.HttpStatusCodeAttribute)

		if code != 200:
			msg = self.versionCheckResponse.attribute(QNetworkRequest.HttpReasonPhraseAttribute)
			self.onFail(f'Update check failed [{code} {msg}]')
			return

		latestVersion = str(self.versionCheckResponse.readAll(), 'utf-8').strip()
		if latestVersion == '':
			self.window.setStatus(f'You\'re up to date!')
			self.launch()
			return

		self.window.setStatus(f'Latest version is {latestVersion}')
		
		latestVersionPath = Path(f'Updates/{latestVersion}')
		if latestVersionPath.exists() and latestVersionPath.is_file():
			self.window.setStatus(f'You\'re up to date!')
			self.launch()
			return

		self.window.setStatus(f'Downloading update...')

		self.downloadPath = Path(f'updates/{latestVersion}.zip')
		self.downloadPath.parent.mkdir(exist_ok=True)
		self.downloadFileHandle = self.downloadPath.open('wb')
		self.updateDownloadRequest = QNetworkRequest()
		
		url = f'http://www.greenlightgo.org/vtlearn/{latestVersion}-{platform.system().lower()}.zip'
		print(url)
		self.updateDownloadRequest.setUrl(QUrl(url))

		self.updateDownloadResponse = self.downloader.get(self.updateDownloadRequest)
		self.updateDownloadResponse.readyRead.connect(self.onUpdateDownloadReadyRead)
		self.updateDownloadResponse.finished.connect(self.onUpdateDownloadComplete)
		
	def onUpdateDownloadReadyRead(self):
		availableBytes = self.updateDownloadResponse.bytesAvailable()
		if availableBytes > 0:
			if self.updateDownloadResponse.hasRawHeader(b'Content-Length'):
				length = self.updateDownloadResponse.header(QNetworkRequest.KnownHeaders.ContentLengthHeader)
				self.window.setProgressMax(length)

			self.window.addProgress(availableBytes)
			self.downloadFileHandle.write(bytes(self.updateDownloadResponse.read(availableBytes)))

	def onUpdateDownloadComplete(self):
		self.downloadFileHandle.close()

		code = self.updateDownloadResponse.attribute(QNetworkRequest.HttpStatusCodeAttribute)

		if code != 200:
			msg = self.updateDownloadResponse.attribute(QNetworkRequest.HttpReasonPhraseAttribute)
			self.onFail(f'Update download failed [{code} {msg}]')
			return

		self.window.setProgressMax(0)
		self.window.setStatus(f'Unpacking update...')

		self.unzipProc = Process(target=apply_update, args=(self.downloadPath,))
		self.unzipProc.start()
		self.zipWatcher = QTimer()
		self.zipWatcher.timeout.connect(self.pollZipProc)
		self.zipWatcher.setInterval(1000)
		self.zipWatcher.start()

	def pollZipProc(self):
		if not self.unzipProc.is_alive():
			self.zipWatcher.stop()

			if self.unzipProc.exitcode == 0:
				self.window.setStatus(f'Update applied!')
				self.launch()
			else:
				self.onFail(f'Failed to apply update [{self.unzipProc.exitcode}]')

	def onFail(self, message):
		self.window.setError(f'Error: {message}')

	def launch(self, delay=1000):
		self.window.setProgressDone()
		QTimer.singleShot(delay, self.launchImmediately)

	def launchImmediately(self):
		self.window.setStatus(f'Launching training app...')
		p = subprocess.Popen(['explorer', 'vtlearn.exe'])

		QTimer.singleShot(3000, self.exit)

if __name__ == '__main__':
	freeze_support()
	app = Bootstrapper()
	app.exec_()
