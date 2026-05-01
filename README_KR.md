> [한국어](README_KR.md) | [English](README.md)
>
> 이 프로젝트는 에이전틱 코딩을 경험해보고자 구현되었습니다.  
> 자세한 내용은 [링크](https://www.facebook.com/share/p/1itHsq4pjY/)를 참조해주세요.

# HyperDOS

HyperDOS는 리얼 모드 DOS와 Windows 3.0 동작을 목표로 하는 C11 기반 IBM PC 호환 에뮬레이터입니다.
현재 1차 목표는 영문 Windows 3.0 리얼 모드가 정확히 동작하도록 16비트 x86 PC 모델, BIOS, 장치, VGA 동작을 맞추는 것입니다.

core 코드는 `src/`와 `include/hyperdos/`에 있으며, Win32 창 처리, 키 입력, 메뉴, 렌더링 같은 플랫폼 계층은 `platforms/win32/`에 있습니다.

## 빌드

```bat
cmake -S . -B cmake-build-debug -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build cmake-build-debug
ctest --test-dir cmake-build-debug --output-on-failure
```

Visual Studio 사용자는 [hyperdos.vcxproj](hyperdos.vcxproj)를 열어 `Debug|x64` 구성으로 빌드할 수 있습니다.

## 실행 예시

```bat
cmake-build-debug\hyperdos_win32_pc_monitor.exe --hard-disk images\harddisk.img --boot-hard-disk
```

운영체제 및 디스크 이미지는 프로젝트에 포함되는 emulator core가 아니며, 사용 권한이 있는 이미지로 테스트해야 합니다.
