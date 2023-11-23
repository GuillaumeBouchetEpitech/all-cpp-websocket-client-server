
const asyncRun = async () => {

  console.log('run');

  const isRelease = process.argv[2] === 'release';

  const config = {
    entrypoints: [`./src/main.ts`],
    outdir: './js',
    target: 'browser',
    format: "esm",
    root: `./src`,
    naming: `[dir]/bundle.[ext]`,
    plugins: [],
  };

  if (isRelease === true) {
    config.minify = {
      whitespace: true,
      identifiers: true,
      syntax: true,
    };
  } else {
    config.sourcemap = "inline";
  }

  const result = await Bun.build(config);

  if (result.success) {
    console.log('BUILD SUCCESS');
  } else {
    console.log('BUILD FAILURE');
    console.log('result', result);
  }

};
asyncRun();
