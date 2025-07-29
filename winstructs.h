#pragma once

#include <Windows.h>

namespace WinStructs
{
    typedef enum _PROCESSINFOCLASS 
    {
        ProcessBasicInformation = 0,
        ProcessQuotaLimits = 1,
        ProcessIoCounters = 2,
        ProcessVmCounters = 3,
        ProcessTimes = 4,
        ProcessBasePriority = 5,
        ProcessRaisePriority = 6,
        ProcessDebugPort = 7,
        ProcessExceptionPort = 8,
        ProcessAccessToken = 9,
        ProcessLdtInformation = 10,
        ProcessLdtSize = 11,
        ProcessDefaultHardErrorMode = 12,
        ProcessIoPortHandlers = 13,
        ProcessPooledUsageAndLimits = 14,
        ProcessWorkingSetWatch = 15,
        ProcessUserModeIOPL = 16,
        ProcessEnableAlignmentFaultFixup = 17,
        ProcessPriorityClass = 18,
        ProcessWx86Information = 19,
        ProcessHandleCount = 20,
        ProcessAffinityMask = 21,
        ProcessPriorityBoost = 22,
        ProcessDeviceMap = 23,
        ProcessSessionInformation = 24,
        ProcessForegroundInformation = 25,
        ProcessWow64Information = 26,
        ProcessImageFileName = 27,
        ProcessLUIDDeviceMapsEnabled = 28,
        ProcessBreakOnTermination = 29,
        ProcessDebugObjectHandle = 30,
        ProcessDebugFlags = 31,
        ProcessHandleTracing = 32,
        MaxProcessInfoClass
    } PROCESSINFOCLASS;

    typedef struct _SYSTEM_KERNEL_DEBUGGER_INFORMATION_EX
    {
        BOOLEAN DebuggerAllowed;
        BOOLEAN DebuggerEnabled;
        BOOLEAN DebuggerPresent;
    } SYSTEM_KERNEL_DEBUGGER_INFORMATION_EX, * PSYSTEM_KERNEL_DEBUGGER_INFORMATION_EX;

    // Source: https://www.vergiliusproject.com/kernels/x64/windows-11/24h2/_STRING64
    struct _STRING64
    {
        USHORT Length;                                                          //0x0
        USHORT MaximumLength;                                                   //0x2
        ULONGLONG Buffer;                                                       //0x8
    };

    // Source: https://www.vergiliusproject.com/kernels/x64/windows-11/24h2/_PEB64
    struct _PEB64
    {
        UCHAR InheritedAddressSpace;                                            //0x0
        UCHAR ReadImageFileExecOptions;                                         //0x1
        UCHAR BeingDebugged;                                                    //0x2
        union
        {
            UCHAR BitField;                                                     //0x3
            struct
            {
                UCHAR ImageUsesLargePages : 1;                                  //0x3
                UCHAR IsProtectedProcess : 1;                                   //0x3
                UCHAR IsImageDynamicallyRelocated : 1;                          //0x3
                UCHAR SkipPatchingUser32Forwarders : 1;                         //0x3
                UCHAR IsPackagedProcess : 1;                                    //0x3
                UCHAR IsAppContainer : 1;                                       //0x3
                UCHAR IsProtectedProcessLight : 1;                              //0x3
                UCHAR IsLongPathAwareProcess : 1;                               //0x3
            };
        };
        UCHAR Padding0[4];                                                      //0x4
        ULONGLONG Mutant;                                                       //0x8
        ULONGLONG ImageBaseAddress;                                             //0x10
        ULONGLONG Ldr;                                                          //0x18
        ULONGLONG ProcessParameters;                                            //0x20
        ULONGLONG SubSystemData;                                                //0x28
        ULONGLONG ProcessHeap;                                                  //0x30
        ULONGLONG FastPebLock;                                                  //0x38
        ULONGLONG AtlThunkSListPtr;                                             //0x40
        ULONGLONG IFEOKey;                                                      //0x48
        union
        {
            ULONG CrossProcessFlags;                                            //0x50
            struct
            {
                ULONG ProcessInJob : 1;                                         //0x50
                ULONG ProcessInitializing : 1;                                  //0x50
                ULONG ProcessUsingVEH : 1;                                      //0x50
                ULONG ProcessUsingVCH : 1;                                      //0x50
                ULONG ProcessUsingFTH : 1;                                      //0x50
                ULONG ProcessPreviouslyThrottled : 1;                           //0x50
                ULONG ProcessCurrentlyThrottled : 1;                            //0x50
                ULONG ProcessImagesHotPatched : 1;                              //0x50
                ULONG ReservedBits0 : 24;                                       //0x50
            };
        };
        UCHAR Padding1[4];                                                      //0x54
        union
        {
            ULONGLONG KernelCallbackTable;                                      //0x58
            ULONGLONG UserSharedInfoPtr;                                        //0x58
        };
        ULONG SystemReserved;                                                   //0x60
        ULONG AtlThunkSListPtr32;                                               //0x64
        ULONGLONG ApiSetMap;                                                    //0x68
        ULONG TlsExpansionCounter;                                              //0x70
        UCHAR Padding2[4];                                                      //0x74
        ULONGLONG TlsBitmap;                                                    //0x78
        ULONG TlsBitmapBits[2];                                                 //0x80
        ULONGLONG ReadOnlySharedMemoryBase;                                     //0x88
        ULONGLONG SharedData;                                                   //0x90
        ULONGLONG ReadOnlyStaticServerData;                                     //0x98
        ULONGLONG AnsiCodePageData;                                             //0xa0
        ULONGLONG OemCodePageData;                                              //0xa8
        ULONGLONG UnicodeCaseTableData;                                         //0xb0
        ULONG NumberOfProcessors;                                               //0xb8
        ULONG NtGlobalFlag;                                                     //0xbc
        union _LARGE_INTEGER CriticalSectionTimeout;                            //0xc0
        ULONGLONG HeapSegmentReserve;                                           //0xc8
        ULONGLONG HeapSegmentCommit;                                            //0xd0
        ULONGLONG HeapDeCommitTotalFreeThreshold;                               //0xd8
        ULONGLONG HeapDeCommitFreeBlockThreshold;                               //0xe0
        ULONG NumberOfHeaps;                                                    //0xe8
        ULONG MaximumNumberOfHeaps;                                             //0xec
        ULONGLONG ProcessHeaps;                                                 //0xf0
        ULONGLONG GdiSharedHandleTable;                                         //0xf8
        ULONGLONG ProcessStarterHelper;                                         //0x100
        ULONG GdiDCAttributeList;                                               //0x108
        UCHAR Padding3[4];                                                      //0x10c
        ULONGLONG LoaderLock;                                                   //0x110
        ULONG OSMajorVersion;                                                   //0x118
        ULONG OSMinorVersion;                                                   //0x11c
        USHORT OSBuildNumber;                                                   //0x120
        USHORT OSCSDVersion;                                                    //0x122
        ULONG OSPlatformId;                                                     //0x124
        ULONG ImageSubsystem;                                                   //0x128
        ULONG ImageSubsystemMajorVersion;                                       //0x12c
        ULONG ImageSubsystemMinorVersion;                                       //0x130
        UCHAR Padding4[4];                                                      //0x134
        ULONGLONG ActiveProcessAffinityMask;                                    //0x138
        ULONG GdiHandleBuffer[60];                                              //0x140
        ULONGLONG PostProcessInitRoutine;                                       //0x230
        ULONGLONG TlsExpansionBitmap;                                           //0x238
        ULONG TlsExpansionBitmapBits[32];                                       //0x240
        ULONG SessionId;                                                        //0x2c0
        UCHAR Padding5[4];                                                      //0x2c4
        _ULARGE_INTEGER AppCompatFlags;                                         //0x2c8
        _ULARGE_INTEGER AppCompatFlagsUser;                                     //0x2d0
        ULONGLONG pShimData;                                                    //0x2d8
        ULONGLONG AppCompatInfo;                                                //0x2e0
        _STRING64 CSDVersion;                                                   //0x2e8
        ULONGLONG ActivationContextData;                                        //0x2f8
        ULONGLONG ProcessAssemblyStorageMap;                                    //0x300
        ULONGLONG SystemDefaultActivationContextData;                           //0x308
        ULONGLONG SystemAssemblyStorageMap;                                     //0x310
        ULONGLONG MinimumStackCommit;                                           //0x318
        ULONGLONG SparePointers[2];                                             //0x320
        ULONGLONG PatchLoaderData;                                              //0x330
        ULONGLONG ChpeV2ProcessInfo;                                            //0x338
        ULONG AppModelFeatureState;                                             //0x340
        ULONG SpareUlongs[2];                                                   //0x344
        USHORT ActiveCodePage;                                                  //0x34c
        USHORT OemCodePage;                                                     //0x34e
        USHORT UseCaseMapping;                                                  //0x350
        USHORT UnusedNlsField;                                                  //0x352
        ULONGLONG WerRegistrationData;                                          //0x358
        ULONGLONG WerShipAssertPtr;                                             //0x360
        ULONGLONG EcCodeBitMap;                                                 //0x368
        ULONGLONG pImageHeaderHash;                                             //0x370
        union
        {
            ULONG TracingFlags;                                                 //0x378
            struct
            {
                ULONG HeapTracingEnabled : 1;                                   //0x378
                ULONG CritSecTracingEnabled : 1;                                //0x378
                ULONG LibLoaderTracingEnabled : 1;                              //0x378
                ULONG SpareTracingBits : 29;                                    //0x378
            };
        };
        UCHAR Padding6[4];                                                      //0x37c
        ULONGLONG CsrServerReadOnlySharedMemoryBase;                            //0x380
        ULONGLONG TppWorkerpListLock;                                           //0x388
        struct LIST_ENTRY64 TppWorkerpList;                                     //0x390
        ULONGLONG WaitOnAddressHashTable[128];                                  //0x3a0
        ULONGLONG TelemetryCoverageHeader;                                      //0x7a0
        ULONG CloudFileFlags;                                                   //0x7a8
        ULONG CloudFileDiagFlags;                                               //0x7ac
        CHAR PlaceholderCompatibilityMode;                                      //0x7b0
        CHAR PlaceholderCompatibilityModeReserved[7];                           //0x7b1
        ULONGLONG LeapSecondData;                                               //0x7b8
        union
        {
            ULONG LeapSecondFlags;                                              //0x7c0
            struct
            {
                ULONG SixtySecondEnabled : 1;                                   //0x7c0
                ULONG Reserved : 31;                                            //0x7c0
            };
        };
        ULONG NtGlobalFlag2;                                                    //0x7c4
        ULONGLONG ExtendedFeatureDisableMask;                                   //0x7c8
    };
}