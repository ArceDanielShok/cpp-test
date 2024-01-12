const addon = require("./build/Release/watcher");
const path = require("path");
function handleFolderChanges(changeInfo) {
  const { fileName, changeType } = changeInfo;

  switch (changeType) {
    case "ADDED":
      console.log(`El archivo ${fileName} fue agregado.`);
      break;
    case "DELETED":
      console.log(`El archivo ${fileName} fue eliminado.`);
      break;
    case "MODIFIED":
      console.log(`El archivo ${fileName} fue modificado.`);
      break;
    default:
  }
}
console.log(`Iniciando...`);

process.on("unhandledRejection", (reason, p) => {
  console.error("Unhandled Rejection at:", p, "reason:", reason);
});

const folderPath = path.join(__dirname, "test");
// addon.startWatching(folderPath, handleFolderChanges);

console.log("Presiona Ctrl + C para detener el script.");
process.stdin.resume();

process.on("unhandledRejection", (reason, p) => {
  console.error("Unhandled Rejection at:", p, "reason:", reason);
});
