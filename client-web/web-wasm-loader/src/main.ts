
import Logger from "./Logger";
import { Application } from "./Application";

const findHtmlElementOrFail = <T extends Element>(inElementId: string): T => {
  const textAreaElement = document.querySelector<T>(inElementId);
  if (!textAreaElement)
    throw new Error(`DOM elements not found, id: "${inElementId}"`);
  return textAreaElement;
}

const getWebSocketConfig = async (logger: Logger): Promise<string> => {
  try {
    const response = await fetch('/web-socket-config.json');
    const result = await response.json();
    logger.log(`[JS] config from server: "${JSON.stringify(result)}"`);
    return result.webSocketUrl;
  }
  catch (err) {
    logger.error(`[JS] fail to fetch the websocket config from server`);
    logger.error(`[JS] -> message: "${err}"`);
    throw err;
  }
}

const onGlobalPageLoad = async () => {

  const textAreaElement = findHtmlElementOrFail<HTMLTextAreaElement>("#loggerOutput");
  const logger = new Logger(textAreaElement);

  logger.log("[JS] page loaded");

  const onGlobalPageError = (event: ErrorEvent) => {
    logger.error(`[JS] exception, event=${event}`);
  };
  window.addEventListener("error", onGlobalPageError);

  //
  //
  // fetch websocket config

  const websocketUrl = await getWebSocketConfig(logger);
  logger.log(`[JS] websocketUrl: "${websocketUrl}"`);

  //
  //
  // start wasm application

  const onProgress = (percent: number) => {
    const statusMsg = `Loading wasm [${percent}%]`;

    // remove the last logged entry if it was a progress updated
    if (logger.size > 0 && logger.peekLast()!.indexOf("Loading wasm [") >= 0)
      logger.popLast();

    logger.log(`[JS] ${statusMsg}`);
  };

  const myApplication = new Application(onProgress);

  try {
    await myApplication.initialize(websocketUrl, logger);
  }
  catch (err) {
    logger.error(`[JS] failed to initialize the wasm application`);
    logger.error(`[JS] -> message: "${err.message}"`);
    throw err;
  }
};

window.addEventListener("load", onGlobalPageLoad);
