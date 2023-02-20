
import Logger from "./Logger";
import {
  scriptLoadingUtility,
} from "./helpers/index";
import {
  isWasmSupported,
} from "./environment/index";

export class Application {

  private _isInitialized: boolean = false;
  private _isAborted: boolean = false;
  private _onProgress: (percent: number) => void;

  constructor(
    onProgress: (percent: number) => void,
  ) {
    this._onProgress = onProgress;
  }

  async initialize(
    wsUrl: string,
    inLogger: Logger,
  ): Promise<void> {

    //
    //
    // WebAssembly support

    if (!isWasmSupported()) {
      throw new Error("missing WebAssembly feature (unsupported)");
    }

    inLogger.log("[JS][check] WebAssembly feature => supported");

    //
    //
    // Setup

    const wasmFolder = "wasm";

    return new Promise((resolve, reject) => {

      //
      //
      // setup the wasm module

      const Module = {
        downloadingDataRegExp: /Downloading data\.\.\. \(([0-9]*)\/([0-9]*)\)/,
        lastProgressLevel: 0,
        locateFile: (url: string) => { return `${wasmFolder}/${url}`; },
        print: (text: string) => { inLogger.log(`[C++][out] ${text}`); },
        printErr: (text: string) => { inLogger.error(`[C++][err] ${text}`); },
        setStatus: (text: string) => {

          if (!text)
            return;

          // is the current message a "Downloading data..." one?
          const capture = Module.downloadingDataRegExp.exec(text);
          if (capture) {

            const current = parseFloat(capture[1]);
            const total = parseFloat(capture[2]);
            const percent = Math.floor((current / total) * 100);

            if (Module.lastProgressLevel !== percent) {
              Module.lastProgressLevel = percent;
              this._onProgress(percent);
            }
          }
          else {
            inLogger.log(`[JS][wasm][status] ${text}`);
          }
        },
        onRuntimeInitialized: () => {

          inLogger.log("[JS][wasm] loaded");
          inLogger.log("[JS][wasm] initializing");

          const wasmFunctions = {
            startApplication: (window as any).Module.cwrap('startApplication', undefined, ['number']),
          };

          const strPtr = (window as any).Module.allocateUTF8(wsUrl);

          wasmFunctions.startApplication(strPtr);

          (window as any).Module._free(strPtr);

          this._isInitialized = true;

          inLogger.log("[JS][wasm] initialized");

          resolve();
        },
        noInitialRun: true,
        noExitRuntime: true,
      };

      // this is needed by the wasm side
      (window as any).Module = Module;

      inLogger.log("[JS][wasm] loading");
      scriptLoadingUtility(`./${wasmFolder}/client.js`).catch(reject);
    });
  }

  abort(): void {

    if (!this._isInitialized || this._isAborted)
      return;

    this._isAborted = true;
    const currModule = (window as any).Module;
    if (currModule) {
      currModule.setStatus = (text: string) => {
        if (text)
          console.error(`[JS][wasm][aborted] ${text}`);
      };
    }
  }
};
