import Logger from './Logger';
import { scriptLoadingUtility } from './helpers/index';
import { isWasmSupported, isWebSocketSupported } from './environment/index';

//
//
//

interface IModuleArgs {
  locateFile: (url: string) => string;
  print: (text: string) => void;
  printErr: (text: string) => void;
  setStatus: (text: string) => void;
  onRuntimeInitialized: () => void;
  noInitialRun: boolean;
  noExitRuntime: boolean;
}

interface IMyModule extends IModuleArgs {
  cwrap: (name: string, returnVal: string | undefined, args: string[]) => any;
  allocateUTF8: (value: string) => number;
  _free: (value: number) => void;
}

declare function clientWeb(args: IModuleArgs): Promise<IMyModule>

//
//
//

export class Application {
  private _isInitialized: boolean = false;
  private _isAborted: boolean = false;
  private _onProgress: (percent: number) => void;

  private _module: IMyModule;

  private _wasmApplicationStartFunc: (inHostStrPtr: number, inPortStrPtr: number) => void;

  constructor(onProgress: (percent: number) => void) {
    this._onProgress = onProgress;
  }

  async initialize(host: string, port: string, inLogger: Logger): Promise<void> {
    //
    //
    // WebAssembly support

    if (!isWasmSupported()) {
      throw new Error('missing WebAssembly feature (unsupported)');
    }
    inLogger.log('[JS][check] WebAssembly feature => supported');

    if (!isWebSocketSupported()) {
      throw new Error('missing WebSocket feature (unsupported)');
    }
    inLogger.log('[JS][check] WebSocket feature => supported');

    await this._setupWasmApplication(host, port, inLogger);
  }

  private async _fetchWasmScript(wasmFolder: string, inLogger: Logger): Promise<void> {
    inLogger.log('[JS][wasm] fetching');
    const fetchStartTime = Date.now();

    // this will globally expose the wasm functions
    await scriptLoadingUtility(`./${wasmFolder}/index.js`);

    const fetchStopTime = Date.now();
    const fetchElapsedTime = ((fetchStopTime - fetchStartTime) / 1000).toFixed(3);
    inLogger.log(`[JS][wasm] fetched ${fetchElapsedTime}sec`);
  }

  private async _loadWasmApplication(wasmFolder: string, inLogger: Logger): Promise<void> {
    inLogger.log('[JS] loading');
    const loadStartTime = Date.now();

    const downloadingDataRegExp = /Downloading data\.\.\. \(([0-9]*)\/([0-9]*)\)/;
    let lastProgressLevel = 0;

    const moduleArgs: IModuleArgs = {
      locateFile: (url: string) => `${wasmFolder}/${url}`,
      print: (text: string) => {
        inLogger.log(`[C++][out] ${text}`);
      },
      printErr: (text: string) => {
        inLogger.error(`[C++][err] ${text}`);
      },
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

        if (lastProgressLevel <= percent) {
          return;
        }

        lastProgressLevel = percent;
        this._onProgress(percent);
      },
      onRuntimeInitialized: () => {
        inLogger.log('[JS][wasm] runtime initialized');
      },
      noInitialRun: true,
      noExitRuntime: true
    };

    // "clientWeb" -> from the wasm module, defined in the Makefile
    this._module = await clientWeb(moduleArgs);

    const loadStopTime = Date.now();
    const loadElapsedTime = ((loadStopTime - loadStartTime) / 1000).toFixed(3);
    inLogger.log(`[JS] loaded ${loadElapsedTime}sec`);
  }

  private _initializeWasmApplication(inLogger: Logger): void {
    inLogger.log('[JS][wasm] initializing');
    const initStartTime = Date.now();

    const wasmFunctions = {
      startApplication: this._module.cwrap('startApplication', undefined, ['number', 'number']),
    };

    this._wasmApplicationStartFunc = wasmFunctions.startApplication;

    this._isInitialized = true;

    const initStopTime = Date.now();
    const initElapsedTime = ((initStopTime - initStartTime) / 1000).toFixed(3);
    inLogger.log(`[JS][wasm] initialized ${initElapsedTime}sec`);
  }

  private async _setupWasmApplication(host: string, port: string, inLogger: Logger): Promise<void> {
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

    const hostStrPtr = this._module.allocateUTF8(host);
    const portStrPtr = this._module.allocateUTF8(port);

    this._wasmApplicationStartFunc(hostStrPtr, portStrPtr);

    this._module._free(hostStrPtr);
    this._module._free(portStrPtr);

    inLogger.log('[JS][wasm] running');
  }

  abort(): void {
    if (!this._isInitialized || this._isAborted) {
      return;
    }

    this._isAborted = true;
    const currModule = this._module;
    if (currModule) {
      currModule.setStatus = (text: string) => {
        if (text) {
          console.error(`[JS][wasm][aborted] ${text}`);
        }
      };
    }
  }
}
