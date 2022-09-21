#include <emscripten.h>

#include <StftPitchShift/CLI.h>
#include <StftPitchShift/StftPitchShift.h>
#include <StftPitchShift/Version.h>

using namespace stftpitchshift;

void INFO(const std::string& message)
{
  EM_ASM({ console.info(UTF8ToString($0)); }, message.c_str());
}

void ERROR(const std::string& message)
{
  EM_ASM({ console.error(UTF8ToString($0)); }, message.c_str());
}

EM_JS(void, shiftpitch_version, (),
{
  var size = Module._shiftpitch_version_str(0, 0);
  var data = Module._malloc(size + 1);

  Module._shiftpitch_version_str(data, size);
  var value = UTF8ToString(data);
  Module._free(data);

  return value;
});

EM_JS(void, shiftpitch_help, (),
{
  var size = Module._shiftpitch_help_str(0, 0);
  var data = Module._malloc(size + 1);

  Module._shiftpitch_help_str(data, size);
  var value = UTF8ToString(data);
  Module._free(data);

  return value;
});

EM_JS(void, shiftpitch, (int buffer, int args),
{
  console.assert(typeof(buffer) === 'object', buffer);
  console.assert(Object.prototype.toString.call(buffer).slice(8, -1) === 'AudioBuffer', buffer);
  console.assert((typeof(args) === 'undefined') || (typeof(args) === 'string'), args);

  var samplerate = buffer.sampleRate;
  var samples = buffer.length;
  var channels = buffer.numberOfChannels;

  var meminput = Module._malloc(samples * Float32Array.BYTES_PER_ELEMENT);
  var memoutput = Module._malloc(samples * Float32Array.BYTES_PER_ELEMENT);

  var input = new Float32Array(Module.HEAPF32.buffer, meminput, samples);
  var output = new Float32Array(Module.HEAPF32.buffer, memoutput, samples);

  var memargs = allocateUTF8(args ? args : "");

  for (var i = 0; i < channels; i++)
  {
    buffer.copyFromChannel(input, i);

    if (!Module._shiftpitch_f32(samplerate, samples, meminput, memoutput, memargs))
    {
      break;
    }

    buffer.copyToChannel(output, i);
  }

  Module._free(memargs);
  Module._free(meminput);
  Module._free(memoutput);

  return buffer;
});

extern "C"
{
  int EMSCRIPTEN_KEEPALIVE shiftpitch_version_str(char* data, int size)
  {
    const std::string value = StftPitchShiftVersion;

    value.copy(data, size);

    return value.size();
  }

  int EMSCRIPTEN_KEEPALIVE shiftpitch_help_str(char* data, int size)
  {
    const std::string value = CLI("-h").usage();

    value.copy(data, size);

    return value.size();
  }

  bool EMSCRIPTEN_KEEPALIVE shiftpitch_f32(double samplerate, int samples, float* input, float* output, char* args)
  {
    CLI cli(args);

    if (cli.help)
    {
      INFO(cli.usage());
      return false;
    }

    if (cli.version)
    {
      INFO(StftPitchShiftVersion);
      return false;
    }

    try
    {
      StftPitchShift stft(
        cli.framesize,
        cli.hoprate ? cli.framesize / cli.hoprate : 1,
        samplerate,
        cli.normalization,
        cli.chronometry);

      stft.shiftpitch(
        samples,
        input,
        output,
        cli.factors,
        cli.quefrency,
        cli.distortion);
    }
    catch (const std::exception& exception)
    {
      ERROR(exception.what());
      return false;
    }

    return true;
  }
}
