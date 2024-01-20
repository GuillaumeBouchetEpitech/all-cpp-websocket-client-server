import Logger from './Logger';
import { scriptLoadingUtility } from './helpers/index';
import { isWasmSupported } from './environment/index';

export class Application {
  private _isInitialized: boolean = false;
  private _isAborted: boolean = false;
  private _onProgress: (percent: number) => void;

  private _module: any;

  private _wasmApplicationStartFunc: (inUrlStrPtr: number) => void;

  constructor(onProgress: (percent: number) => void) {
    this._onProgress = onProgress;
  }

  async initialize(wsUrl: string, inLogger: Logger): Promise<void> {
    //
    //
    // WebAssembly support

    if (!isWasmSupported()) {
      throw new Error('missing WebAssembly feature (unsupported)');
    }

    inLogger.log('[JS][check] WebAssembly feature => supported');

    await this._setupWasmApplication(wsUrl, inLogger);
  }

  private async _fetchWasmScript(
    wasmFolder: string,
    inLogger: Logger,
  ): Promise<void> {

    inLogger.log("[JS][wasm] fetching");
    const fetchStartTime = Date.now();

    // this will globally expose the function clientWeb()
    await scriptLoadingUtility(`./${wasmFolder}/client.js`);

    const fetchStopTime = Date.now();
    const fetchElapsedTime = ((fetchStopTime - fetchStartTime) / 1000).toFixed(3);
    inLogger.log(`[JS][wasm] fetched ${fetchElapsedTime}sec`);
  }

  private async _loadWasmApplication(
    wasmFolder: string,
    inLogger: Logger,
  ): Promise<void> {

    inLogger.log("[JS] loading");
    const loadStartTime = Date.now();

    const downloadingDataRegExp = /Downloading data\.\.\. \(([0-9]*)\/([0-9]*)\)/;
    let lastProgressLevel = 0;

    const moduleArgs = {
      locateFile: (url: string) => `${wasmFolder}/${url}`,
      print: (text: string) => { inLogger.log(`[C++][out] ${text}`); },
      printErr: (text: string) => { inLogger.error(`[C++][err] ${text}`); },
      setStatus: (text: string) => {

        if (!text) {
          return;
        }

        // is the current message a "Downloading data..." one?
        const capture = downloadingDataRegExp.exec(text);
        if (!capture) {
          // no -> just a regular status message
          inLogger.log(`[JS][wasm][status] ${text}`);
          return;
        }

        const current = parseFloat(capture[1]);
        const total = parseFloat(capture[2]);
        const percent = Math.floor((current / total) * 100);

        if (lastProgressLevel === percent) {
          return;
        }

        lastProgressLevel = percent;
        this._onProgress(percent);
      },
      onRuntimeInitialized: () => {
        inLogger.log("[JS][wasm] runtime initialized");
      },
      noInitialRun: true,
      noExitRuntime: true,
    };

    // @ts-ignore
    this._module = await clientWeb(moduleArgs);

    const loadStopTime = Date.now();
    const loadElapsedTime = ((loadStopTime - loadStartTime) / 1000).toFixed(3);
    inLogger.log(`[JS] loaded ${loadElapsedTime}sec`);

  }

  private _initializeWasmApplication(inLogger: Logger): void {

    inLogger.log("[JS][wasm] initializing");
    const initStartTime = Date.now();

    const wasmFunctions = {
      startApplication: this._module.cwrap('startApplication', undefined, ['number']),
    };

    this._wasmApplicationStartFunc = wasmFunctions.startApplication;

    this._isInitialized = true;

    const initStopTime = Date.now();
    const initElapsedTime = ((initStopTime - initStartTime) / 1000).toFixed(3);
    inLogger.log(`[JS][wasm] initialized ${initElapsedTime}sec`);

  }

  private async _setupWasmApplication(
    wsUrl: string, inLogger: Logger,
  ): Promise<void> {

    const wasmFolder = `wasm`;

    //
    //
    //

    await this._fetchWasmScript(wasmFolder, inLogger);
    await this._loadWasmApplication(wasmFolder, inLogger);
    this._initializeWasmApplication(inLogger);

    //
    //
    //

    const strPtr = this._module.allocateUTF8(wsUrl);

    this._wasmApplicationStartFunc(strPtr);

    this._module._free(strPtr);

    inLogger.log("[JS][wasm] running");
  }


  abort(): void {
    if (!this._isInitialized || this._isAborted) return;

    this._isAborted = true;
    const currModule = this._module;
    if (currModule) {
      currModule.setStatus = (text: string) => {
        if (text) console.error(`[JS][wasm][aborted] ${text}`);
      };
    }
  }
}
