export const isWebSocketSupported = (): boolean => {
  return (typeof(WebSocket) === 'object');
};
