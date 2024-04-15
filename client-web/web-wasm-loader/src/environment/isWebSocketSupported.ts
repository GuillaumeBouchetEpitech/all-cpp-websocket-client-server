export const isWebSocketSupported = (): boolean => {
  return (window.WebSocket !== undefined);
};
