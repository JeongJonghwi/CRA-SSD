# SSD Test Project

Visual Studio 2022 기반의 SSD 시뮬레이션 및 테스트 프레임워크입니다.

## 📁 프로젝트 구성
```
CRA-SSD
├─include # TestShell 및 TestScript에서 공유되는 인터페이스 헤더
├─SSD # SSD 시뮬레이터 (실행 파일)
├─TestShell # 사용자 입력 기반 테스트 셸 (실행 파일)
├─TestScript # 테스트 스크립트 모듈 (DLL)
├─LoggerLib # 공용 로깅 라이브러리 (정적 라이브러리)
├─packages
│  └─gmock.1.11.0   # 유닛테스트 라이브러리 
```

## 🛠️ 개발 환경

- IDE: Visual Studio 2022
- 언어: C++
- 빌드 시스템: MSBuild (Visual Studio 솔루션 기반)
- 대상 플랫폼: Windows x64


## 🔧 빌드 및 실행 방법

1. 아래와 같이 프로젝트를 클론합니다. 
```bash
git clone https://github.com/JeongJonghwi/CRA-SSD.git
cd CRA-SSD
```
2. Visual Studio 2022로 `CRA-C_SSD_Project.sln` 파일을 엽니다.
3. 솔루션 구성 (`Release` 또는 `Debug`) 및 플랫폼 (`x64`)을 선택합니다.
4. **전체 솔루션 빌드** (메뉴에서 `빌드 > 솔루션 빌드` 또는 `Ctrl+Shift+B`)를 실행합니다.
5. 실행은 아래와 같이 명령줄에서 직접 실행하는 방식을 권장합니다:
- Debug 모드
  ```
  cd x64/Debug
  .\TestShell.exe   # 또는 SSD.exe
  ```
- Release 모드
  ```
  cd x64/Release
  .\TestShell.exe   # 또는 SSD.exe
  ```
**⚠️ 주의:** Visual studio에서 `F5` 또는 `Ctrl+F5`로 실행 시, SSD 출력 파일의 경로 문제로 인해 유닛 테스트가 실패하거나 비정상적인 결과가 발생할 수 있습니다. 반드시 위에 안내된 명령줄 방식으로 실행해 주세요.


## 🖥️ SSD 사용법 

| 명령어 | 설명 |
| -----  | ----- |
| W {주소} {값} | 지정한 {주소}에 {값}을 기록합니다. 예: W 10 0x12345678 |
| R {주소} | 지정한 {주소}에서 값을 읽습니다. 예: R 10 |
| E {주소} {크기} | 지정한 {주소}에서 {크기} 만큼의 영역들의 값을 0x00000000 값으로 재설정합니다. 예: E 0 2 |

ℹ️ **참고:**  값은 0x 로 시작하는 hex string 이며, 0x를 제외하고 정확히 8글자의 string으로 표현되어야 합니다.  


## 🖥️ TestShell 사용법 


| 명령어 | 설명 |
| -----  | ----- |
| write {주소} {값} | 지정한 {주소}에 {값}을 기록합니다. 예: write 10 0x12345678 |
| read {주소} | 지정한 {주소}에서 값을 읽습니다. 예: read 10 |
| erase {주소} {크기} | 지정한 {주소}에서 {크기} 만큼의 영역들의 값을 0x00000000 값으로 재설정합니다. 예: erase 0 2 |
| erase_range {시작 주소} {종료 주소} | 지정한 {시작 주소}에서 {종료 주소} 까지의 영역들의 값을 0x00000000 값으로 재설정합니다. 예: erase_range 4 5 |
| fullwrite {값} | 주소 0부터 99까지 모든 주소에 {값}을 기록합니다. |
| fullread | 주소 0부터 99까지 모든 값을 읽어 출력합니다. |
| help | 사용 가능한 명령어 목록을 출력합니다. |
| exit | TestShell을 종료합니다. |

ℹ️ **참고:**  값은 0x 로 시작하는 hex string 이며, 0x를 제외하고 정확히 8글자의 string으로 표현되어야 합니다.  

## 🧪 TestScript

`TestScript` 프로젝트는 DLL 형식으로 빌드되며, `ITestScript` 인터페이스를 상속받는 다양한 테스트 시나리오 클래스를 포함합니다.

예시:
- `FullWriteAndReadCompareScript`
- `PartialWriteAndReadCompareScript`
- `WriteReadAgingScript`
- `EraseAndWriteAgingScript`

이 DLL은 런타임에 `TestShell`에서 로딩되며, `include/` 디렉토리에 정의된 공용 헤더를 통해 인터페이스가 공유됩니다.
`TestScript`는 플러그인 방식의 확장 구조를 가지며, 새로운 테스트를 추가하려면 `ITestScript`를 상속한 새 클래스를 정의하고 DLL에 포함시키면 됩니다.

## 📝 참고

- `LoggerLib`는 정적 라이브러리로, 모든 프로젝트에서 공통적으로 사용됩니다.
