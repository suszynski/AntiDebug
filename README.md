# AntiDebug

## What is this?

A simple terminal interface tool to test Windows x86_64 anti-debugging techniques. Like many of our public projects this was made because we needed it to better explain
some of [our courses](https://haxo.games/courses).

## Build

All this requires is Visual Studio 2022 (minimum) and the "Desktop Development with C++" package installed. The only dependencies this project uses are the Windows API 
and [FTXUI](https://github.com/ArthurSonzogni/FTXUI). FTXUI is directly included within the `dependencies/` folder. I'm aware this is bad practice, but I want to keep
the build environment as simplistic and similar to what is used throughout the course as possible for beginners.

## Usage

Simply run the executable and attach your debugger to the process. The console is conveniently titled "AntiDebug" on launch for you to easily find it. You can toggle
detections by navigating to them using your up and down arrow keys and pressing `ENTER` or `SPACE`. You can terminate the program with `Q`. Dections checks run continuously
when enabled so just experiment and see what detects you, good luck!

![Screenshot of AntiDebug running.](repo/preview.png)

### Adding detections

If you'd like to add detections you first need to create the callback for said detection. To do so head into `antidebug.h`, make its definition with the following format:
`void callbackMyDetectionName(AntiDebugOption& option);`, then head into `antidebug.cpp` and create its declaration like so:
```c++
void AntiDebug::callbackMyDetectionName(AntiDebugOption& option)
{
	// Example detection
	option.detected = IsDebuggerPresent();
}
```
Setting `option.detected` to true or false will instantly update the UI once the detection is fully integrated. You don't need to handle anything else than setting this
value as you wish here. Disable logic and UI events are already handled in the `main` function.
<br />
Now to integrate the detection head into `main.cpp`, locate `options` inside of the `main` function, and add a new member in its initializer list like so:
```c++
AntiDebugOptions options{
	AntiDebugOption("IsDebuggerPresent", true, callbackIsDebuggerPresent),
	AntiDebugOption("MyDetectionName", true, callbackMyDetectionName), // <- Example integration
};
```
The first argument is the title to be displayed in the UI, the second sets if the detection should be enabled or not by default and the third is your callback.
<br />
The last step will be to update the `OPTIONS_COUNT` const expression inside `antidebug.h` to the new list size. I'm aware that this last step is not ideal, but that's the
magic fuckery I figured out for the FTXUI checkboxes atomic state logic to be handled properly.

## Documentation

In this section I will do my best to document the different anti-debugging techniques used, and maybe add extra details like how they can be avoided etc. For any further
information please join [our Discord server](https://discord.gg/f6AbaCATMg).

### `IsDebuggerPresent`

`IsDebuggerPresent` is a Windows API function that simply accesses the `BeingDebugged` boolean from the thread environment block (TEB) of the current process. This value
is true when the process is being debugged. This can be bypassed by simply setting `BeingDebugged` to 0. Patching `IsDebuggerPresent` may seem idiotic since anyone with a
minimum of common sense would not invoke it directly, and yet most programs out there do.

### `BeingDebugged`

This technique directly reads the `BeingDebugged` member of the process environment block (PEB) for the current process. If true, it is being debugged. Can be bypassed by
simply setting the value to 0.

### `CheckRemoteDebuggerPresent`

`CheckRemoteDebuggerPresent` is a Windows API function which which calls `NtQueryInformationProcess` for the `ProcessDebugPort` value under the hood. If this value is 0
no debugger is attached, otherwise it sets the pointer to the boolean it was passed to true. This can be bypassed by hooking `NtQueryInformationProcess` or the function
itself. `ProcessDebugPort` being in the `EPROCESS` structure it is immune to usermode shenanigans.

### `NtQueryInformationProcess`

`NtQueryInformationProcess` can be used to get `ProcessDebugPort` which can indicate if a debugger is present. It can also query a few values in an undocumented way such
as the heap flags (`ProcessDebugFlags`) and the debug object handle (`ProcessDebugHandle`).

## Contributing

This project is free and open source and will remain so forever. You are welcome to contribute. Simply make a pull request for whatever it is you would like to add, but
there are a few things you need to keep in mind:
1. C++17 only for now.
2. snake_case for variable names, PascalCase for namespaces and structures and camelCase for methods and function names (there might be more specefics so please just
base your style on the already existing code).
3. Make an issue first, and specify your intent to address said issue.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
