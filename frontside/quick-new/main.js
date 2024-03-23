// Modules to control application life and create native browser window
const { app, BrowserWindow } = require('electron')
const path = require('node:path')

let hotKey = undefined;

if(process.platform == 'win32') {
  //hotKey = require('../app.asar.unpacked/HotkeyLock.node');
  hotKey = require('./HotkeyLock.node');
}else if(process.platform == 'darwin'){ //mac
	
	
}

function setLock(lock) {
  if (process.platform === 'win32') {
    if (lock) {
      return hotKey.lock();
    } else {
      return hotKey.unlock();
    }
  }
}

function createWindow () {
  // Create the browser window.
  const mainWindow = new BrowserWindow({
    width: 800,
    height: 600,
	kiosk: true,//全屏
    webPreferences: {
      preload: path.join(__dirname, 'preload.js')
    }
  })

  // and load the index.html of the app.
  mainWindow.loadFile('index.html')
  mainWindow.setAlwaysOnTop(true)

  // Open the DevTools.
  mainWindow.webContents.openDevTools()
}

// This method will be called when Electron has finished
// initialization and is ready to create browser windows.
// Some APIs can only be used after this event occurs.
app.whenReady().then(() => {
  createWindow()

  app.on('activate', function () {
    // On macOS it's common to re-create a window in the app when the
    // dock icon is clicked and there are no other windows open.
    if (BrowserWindow.getAllWindows().length === 0) createWindow()
  })

  freeze(true)
  setLock(true)
})

// Quit when all windows are closed, except on macOS. There, it's common
// for applications and their menu bar to stay active until the user quits
// explicitly with Cmd + Q.
/*app.on('window-all-closed', function () {
	 var info = setLock(false)
	 var finfo = freeze(false)
	 
	 console.log(info)
	 console.log(finfo)
	 if(info == "success" && finfo == "success"){
		  setTimeout(() => {
			 if (process.platform !== 'darwin') app.quit()
		  }, 500);
	 }
	 
})
*/

app.on('will-quit', function () {
	

});

app.on('window-all-closed', function () {
		  setTimeout(() => {
			 if (process.platform !== 'darwin') app.quit()
		  }, 1000);
});

app.on('quit', function () {

});

app.on('close',function(event){
	
})

app.on('before-quit', function (event) {
	
	 var info = setLock(false)
	 var finfo = freeze(false)
	 
	 console.log(info)
	 console.log(finfo)
})



function freeze(fz) {
  if (process.platform === 'win32') {
    if (fz) {
      return hotKey.doFreeze();
    } else {
      return hotKey.doUnFreeze();
    }
  }
}


// In this file you can include the rest of your app's specific main process
// code. You can also put them in separate files and require them here.
