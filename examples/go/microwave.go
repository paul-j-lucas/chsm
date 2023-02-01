







package main

import (
  "bufio"
  "chsm"
  "fmt"
  "os"
  "unicode"
)



/**
 * Contains additional data and member functions for a microwave CHSM.
 */
type MWData struct {
  chsm.CoreMachine

  minutes, seconds int
  PowerLevel int
  hour, minute int
  old_hour, old_minute int

}

func (mw *MWData) MWDataInit(CHSM_MACHINE_ARGS) {
  chsm.Machine.InitMachine(CHSM_MACHINE_INIT)
  mw.hour = 12
}

func (mw *MWData) OKTime() bool {
  return mw.hour >= 1 && mw.hour <= 12 && mw.minute <= 59
}

func (mw *MWData) DisplayTime() {
  fmt.Printf("Time: %02d:%02d\n", mw.hour, mw.minute)
}

func (mw *MWData) DisplayTimer() {
  fmt.Printf("Timer: %02d:%02d\n", mw.minutes, mw.seconds)
}

func ShiftAdd(victim *int, amount int) {
  *victim = *victim * 10 + amount
  if *victim >= 100 {
    *victim %= 100
  }
}

///// CHSM: <<CHSM 6.0>>

///// CHSM: user-declarations

//#line 235 "microwave.chsmg"
///// CHSM: CHSM struct declaration

type Microwave struct {
	MWData

	// events
	DigitEvent chsm.CoreEvent
	DigitEventPb DigitEventParamBlock
	OpenEvent chsm.CoreEvent
	OpenEventPb OpenEventParamBlock
	MinuteEvent chsm.CoreEvent
	MinuteEventPb MinuteEventParamBlock
	ClockEvent chsm.CoreEvent
	ClockEventPb ClockEventParamBlock
	StartEvent chsm.CoreEvent
	StartEventPb StartEventParamBlock
	StopEvent chsm.CoreEvent
	StopEventPb StopEventParamBlock
	PowerEvent chsm.CoreEvent
	PowerEventPb PowerEventParamBlock
	DoneEvent chsm.CoreEvent
	DoneEventPb DoneEventParamBlock
	CloseEvent chsm.CoreEvent
	CloseEventPb CloseEventParamBlock
	EM10components4mode11operational7program chsm.CoreEvent
	EM10components4mode11operational7programPb EM10components4mode11operational7programParamBlock
	EM10components4mode11operational8set_time5error chsm.CoreEvent
	EM10components4mode11operational8set_time5errorPb EM10components4mode11operational8set_time5errorParamBlock
	EM10components4mode11operational4idle chsm.CoreEvent
	EM10components4mode11operational4idlePb EM10components4mode11operational4idleParamBlock
	EM10components4mode11operational4cook chsm.CoreEvent
	EM10components4mode11operational4cookPb EM10components4mode11operational4cookParamBlock
	XM10components4mode11operational4cook chsm.CoreEvent
	XM10components4mode11operational4cookPb XM10components4mode11operational4cookParamBlock

	// states
	root chsm.CoreCluster
	components struct {
		chsm.CoreSet
		mode struct {
			chsm.CoreCluster
			operational struct {
				chsm.CoreCluster
				idle chsm.CoreState
				program struct {
					chsm.CoreCluster
					setting chsm.CoreState
					power chsm.CoreState
				}
				cook chsm.CoreState
				set_time struct {
					chsm.CoreCluster
					setting chsm.CoreState
					error chsm.CoreState
				}
			}
			disabled chsm.CoreState
		}
		display struct {
			chsm.CoreCluster
			time chsm.CoreState
			not_time struct {
				chsm.CoreCluster
				counter chsm.CoreState
				power chsm.CoreState
				error chsm.CoreState
			}
		}
		light struct {
			chsm.CoreCluster
			off chsm.CoreState
			on chsm.CoreState
		}
	}
}

// New function
func NewMicrowave() *Microwave {
	machine := new(Microwave)

	// initialize events
	machine.DigitEvent.InitEvent(
		machine, "Digit",
		[]chsm.TransitionId{1, 6, 8, 13, 22},
		/*baseEvent=*/nil,
	)
	machine.OpenEvent.InitEvent(
		machine, "Open",
		[]chsm.TransitionId{0},
		/*baseEvent=*/nil,
	)
	machine.MinuteEvent.InitEvent(
		machine, "Minute",
		[]chsm.TransitionId{2, 11},
		/*baseEvent=*/nil,
	)
	machine.ClockEvent.InitEvent(
		machine, "Clock",
		[]chsm.TransitionId{3, 14, 15},
		/*baseEvent=*/nil,
	)
	machine.StartEvent.InitEvent(
		machine, "Start",
		[]chsm.TransitionId{4},
		/*baseEvent=*/nil,
	)
	machine.StopEvent.InitEvent(
		machine, "Stop",
		[]chsm.TransitionId{5, 10, 12},
		/*baseEvent=*/nil,
	)
	machine.PowerEvent.InitEvent(
		machine, "Power",
		[]chsm.TransitionId{7, 21},
		/*baseEvent=*/nil,
	)
	machine.DoneEvent.InitEvent(
		machine, "Done",
		[]chsm.TransitionId{9},
		/*baseEvent=*/nil,
	)
	machine.CloseEvent.InitEvent(
		machine, "Close",
		[]chsm.TransitionId{16},
		/*baseEvent=*/nil,
	)
	machine.EM10components4mode11operational7program.InitEvent(
		machine, "enter(components.mode.operational.program)",
		nil, nil,
	)
	machine.EM10components4mode11operational8set_time5error.InitEvent(
		machine, "enter(components.mode.operational.set_time.error)",
		nil, nil,
	)
	machine.EM10components4mode11operational4idle.InitEvent(
		machine, "enter(components.mode.operational.idle)",
		nil, nil,
	)
	machine.EM10components4mode11operational4cook.InitEvent(
		machine, "enter(components.mode.operational.cook)",
		nil, nil,
	)
	machine.XM10components4mode11operational4cook.InitEvent(
		machine, "exit(components.mode.operational.cook)",
		nil, nil,
	)

	// initialize states
	machine.root.InitCluster(
		&machine.CoreMachine, /*id=*/0, "root",
		/*parent=*/nil,
		/*enterAction=*/nil, /*exitAction=*/nil,
		/*enterEvent=*/nil, /*exitEvent=*/nil,
		[]chsm.State{
			&machine.components,
		},
		/*history=*/false,
	)
	machine.components.InitSet(
		&machine.CoreMachine, /*id=*/1, "components",
		/*parent=*/&machine.root,
		/*enterAction=*/nil, /*exitAction=*/nil,
		/*enterEvent=*/nil, /*exitEvent=*/nil,
		[]chsm.State{
			&machine.components.mode,
			&machine.components.display,
			&machine.components.light,
		},
	)
	machine.components.mode.InitCluster(
		&machine.CoreMachine, /*id=*/2, "components.mode",
		/*parent=*/&machine.components,
		/*enterAction=*/nil, /*exitAction=*/nil,
		/*enterEvent=*/nil, /*exitEvent=*/nil,
		[]chsm.State{
			&machine.components.mode.operational,
			&machine.components.mode.disabled,
		},
		/*history=*/false,
	)
	machine.components.mode.operational.InitCluster(
		&machine.CoreMachine, /*id=*/3, "components.mode.operational",
		/*parent=*/&machine.components.mode,
		/*enterAction=*/nil, /*exitAction=*/nil,
		/*enterEvent=*/nil, /*exitEvent=*/nil,
		[]chsm.State{
			&machine.components.mode.operational.idle,
			&machine.components.mode.operational.program,
			&machine.components.mode.operational.cook,
			&machine.components.mode.operational.set_time,
		},
		/*history=*/true,
	)
	machine.components.mode.operational.idle.InitState(
		&machine.CoreMachine, /*id=*/4, "components.mode.operational.idle",
		/*parent=*/&machine.components.mode.operational,
		/*enterAction=*/nil, /*exitAction=*/nil,
		/*enterEvent=*/&EM10components4mode11operational4idle, /*exitEvent=*/nil,
	)
	machine.components.mode.operational.program.InitCluster(
		&machine.CoreMachine, /*id=*/5, "components.mode.operational.program",
		/*parent=*/&machine.components.mode.operational,
		/*enterAction=*/EAM10components4mode11operational7program, /*exitAction=*/XAM10components4mode11operational7program,
		/*enterEvent=*/&EM10components4mode11operational7program, /*exitEvent=*/nil,
		[]chsm.State{
			&machine.components.mode.operational.program.setting,
			&machine.components.mode.operational.program.power,
		},
		/*history=*/true,
	)
	machine.components.mode.operational.program.setting.InitState(
		&machine.CoreMachine, /*id=*/6, "components.mode.operational.program.setting",
		/*parent=*/&machine.components.mode.operational.program,
		/*enterAction=*/nil, /*exitAction=*/nil,
		/*enterEvent=*/nil, /*exitEvent=*/nil,
	)
	machine.components.mode.operational.program.power.InitState(
		&machine.CoreMachine, /*id=*/7, "components.mode.operational.program.power",
		/*parent=*/&machine.components.mode.operational.program,
		/*enterAction=*/nil, /*exitAction=*/nil,
		/*enterEvent=*/nil, /*exitEvent=*/nil,
	)
	machine.components.mode.operational.cook.InitState(
		&machine.CoreMachine, /*id=*/8, "components.mode.operational.cook",
		/*parent=*/&machine.components.mode.operational,
		/*enterAction=*/nil, /*exitAction=*/nil,
		/*enterEvent=*/&EM10components4mode11operational4cook, /*exitEvent=*/&XM10components4mode11operational4cook,
	)
	machine.components.mode.operational.set_time.InitCluster(
		&machine.CoreMachine, /*id=*/9, "components.mode.operational.set_time",
		/*parent=*/&machine.components.mode.operational,
		/*enterAction=*/EAM10components4mode11operational8set_time, /*exitAction=*/XAM10components4mode11operational8set_time,
		/*enterEvent=*/nil, /*exitEvent=*/nil,
		[]chsm.State{
			&machine.components.mode.operational.set_time.setting,
			&machine.components.mode.operational.set_time.error,
		},
		/*history=*/false,
	)
	machine.components.mode.operational.set_time.setting.InitState(
		&machine.CoreMachine, /*id=*/10, "components.mode.operational.set_time.setting",
		/*parent=*/&machine.components.mode.operational.set_time,
		/*enterAction=*/nil, /*exitAction=*/nil,
		/*enterEvent=*/nil, /*exitEvent=*/nil,
	)
	machine.components.mode.operational.set_time.error.InitState(
		&machine.CoreMachine, /*id=*/11, "components.mode.operational.set_time.error",
		/*parent=*/&machine.components.mode.operational.set_time,
		/*enterAction=*/nil, /*exitAction=*/nil,
		/*enterEvent=*/&EM10components4mode11operational8set_time5error, /*exitEvent=*/nil,
	)
	machine.components.mode.disabled.InitState(
		&machine.CoreMachine, /*id=*/12, "components.mode.disabled",
		/*parent=*/&machine.components.mode,
		/*enterAction=*/nil, /*exitAction=*/nil,
		/*enterEvent=*/nil, /*exitEvent=*/nil,
	)
	machine.components.display.InitCluster(
		&machine.CoreMachine, /*id=*/13, "components.display",
		/*parent=*/&machine.components,
		/*enterAction=*/nil, /*exitAction=*/nil,
		/*enterEvent=*/nil, /*exitEvent=*/nil,
		[]chsm.State{
			&machine.components.display.time,
			&machine.components.display.not_time,
		},
		/*history=*/false,
	)
	machine.components.display.time.InitState(
		&machine.CoreMachine, /*id=*/14, "components.display.time",
		/*parent=*/&machine.components.display,
		/*enterAction=*/nil, /*exitAction=*/nil,
		/*enterEvent=*/nil, /*exitEvent=*/nil,
	)
	machine.components.display.not_time.InitCluster(
		&machine.CoreMachine, /*id=*/15, "components.display.not_time",
		/*parent=*/&machine.components.display,
		/*enterAction=*/nil, /*exitAction=*/nil,
		/*enterEvent=*/nil, /*exitEvent=*/nil,
		[]chsm.State{
			&machine.components.display.not_time.counter,
			&machine.components.display.not_time.power,
			&machine.components.display.not_time.error,
		},
		/*history=*/false,
	)
	machine.components.display.not_time.counter.InitState(
		&machine.CoreMachine, /*id=*/16, "components.display.not_time.counter",
		/*parent=*/&machine.components.display.not_time,
		/*enterAction=*/nil, /*exitAction=*/nil,
		/*enterEvent=*/nil, /*exitEvent=*/nil,
	)
	machine.components.display.not_time.power.InitState(
		&machine.CoreMachine, /*id=*/17, "components.display.not_time.power",
		/*parent=*/&machine.components.display.not_time,
		/*enterAction=*/nil, /*exitAction=*/nil,
		/*enterEvent=*/nil, /*exitEvent=*/nil,
	)
	machine.components.display.not_time.error.InitState(
		&machine.CoreMachine, /*id=*/18, "components.display.not_time.error",
		/*parent=*/&machine.components.display.not_time,
		/*enterAction=*/nil, /*exitAction=*/nil,
		/*enterEvent=*/nil, /*exitEvent=*/nil,
	)
	machine.components.light.InitCluster(
		&machine.CoreMachine, /*id=*/19, "components.light",
		/*parent=*/&machine.components,
		/*enterAction=*/nil, /*exitAction=*/nil,
		/*enterEvent=*/nil, /*exitEvent=*/nil,
		[]chsm.State{
			&machine.components.light.off,
			&machine.components.light.on,
		},
		/*history=*/false,
	)
	machine.components.light.off.InitState(
		&machine.CoreMachine, /*id=*/20, "components.light.off",
		/*parent=*/&machine.components.light,
		/*enterAction=*/nil, /*exitAction=*/nil,
		/*enterEvent=*/nil, /*exitEvent=*/nil,
	)
	machine.components.light.on.InitState(
		&machine.CoreMachine, /*id=*/21, "components.light.on",
		/*parent=*/&machine.components.light,
		/*enterAction=*/nil, /*exitAction=*/nil,
		/*enterEvent=*/nil, /*exitEvent=*/nil,
	)

	// initialize machine
	machine.InitMachine(
		&machine.root,
		[]chsm.State{
			&machine.components,
			&machine.components.mode,
			&machine.components.mode.operational,
			&machine.components.mode.operational.idle,
			&machine.components.mode.operational.program,
			&machine.components.mode.operational.program.setting,
			&machine.components.mode.operational.program.power,
			&machine.components.mode.operational.cook,
			&machine.components.mode.operational.set_time,
			&machine.components.mode.operational.set_time.setting,
			&machine.components.mode.operational.set_time.error,
			&machine.components.mode.disabled,
			&machine.components.display,
			&machine.components.display.time,
			&machine.components.display.not_time,
			&machine.components.display.not_time.counter,
			&machine.components.display.not_time.power,
			&machine.components.display.not_time.error,
			&machine.components.light,
			&machine.components.light.off,
			&machine.components.light.on,
		},
		[]chsm.Transition{
			{nil, &machine.components.mode.operational, nil, nil},
			{nil, &machine.components.mode.operational.idle, nil, nil},
			{nil, &machine.components.mode.operational.idle, nil, A1},
			{nil, &machine.components.mode.operational.idle, nil, nil},
			{nil, &machine.components.mode.operational.program, nil, nil},
			{nil, &machine.components.mode.operational.program, nil, nil},
			{nil, &machine.components.mode.operational.program.setting, nil, A2},
			{nil, &machine.components.mode.operational.program.setting, nil, nil},
			{C1, &machine.components.mode.operational.program.power, nil, A3},
			{nil, &machine.components.mode.operational.cook, nil, nil},
			{nil, &machine.components.mode.operational.cook, nil, nil},
			{nil, &machine.components.mode.operational.cook, nil, A4},
			{nil, &machine.components.mode.operational.set_time, nil, nil},
			{nil, &machine.components.mode.operational.set_time.setting, nil, A5},
			{C2, &machine.components.mode.operational.set_time.setting, nil, nil},
			{C3, &machine.components.mode.operational.set_time.setting, nil, A6},
			{nil, &machine.components.mode.disabled, nil, nil},
			{nil, &machine.components.display.time, nil, nil},
			{nil, &machine.components.display.time, nil, nil},
			{nil, &machine.components.display.not_time, nil, nil},
			{nil, &machine.components.display.not_time, nil, nil},
			{nil, &machine.components.display.not_time.counter, nil, nil},
			{C4, &machine.components.display.not_time.power, nil, nil},
			{nil, &machine.components.light.off, nil, nil},
			{nil, &machine.components.light.on, nil, nil},
		},
	)

	return machine
}

///// CHSM: event definitions

type DigitEventParamBlock struct {
	chsm.CoreEventParamBlock
	n int
}
func (machine *Microwave) Digit(n int) {
	var param DigitEventParamBlock
	param.InitEventParamBlock(&machine.DigitEvent)
	param.n = n
	machine.DigitEvent.Broadcast(&param)
}

type OpenEventParamBlock struct {
	chsm.CoreEventParamBlock
}
func (machine *Microwave) Open() {
	machine.OpenEvent.Broadcast(nil)
}

type MinuteEventParamBlock struct {
	chsm.CoreEventParamBlock
}
func (machine *Microwave) Minute() {
	machine.MinuteEvent.Broadcast(nil)
}

type ClockEventParamBlock struct {
	chsm.CoreEventParamBlock
}
func (machine *Microwave) Clock() {
	machine.ClockEvent.Broadcast(nil)
}

type StartEventParamBlock struct {
	chsm.CoreEventParamBlock
}
func (machine *Microwave) Start() {
	machine.StartEvent.Broadcast(nil)
}

type StopEventParamBlock struct {
	chsm.CoreEventParamBlock
}
func (machine *Microwave) Stop() {
	machine.StopEvent.Broadcast(nil)
}

type PowerEventParamBlock struct {
	chsm.CoreEventParamBlock
}
func (machine *Microwave) Power() {
	machine.PowerEvent.Broadcast(nil)
}

type DoneEventParamBlock struct {
	chsm.CoreEventParamBlock
}
func (machine *Microwave) Done() {
	machine.DoneEvent.Broadcast(nil)
}

type CloseEventParamBlock struct {
	chsm.CoreEventParamBlock
}
func (machine *Microwave) Close() {
	machine.CloseEvent.Broadcast(nil)
}







// user-code
func A1(event chsm.Event) {
	machine := event.Machine().(*Microwave)
	_ = machine
//#line 97 "microwave.chsmg"

            machine.minutes = 1
          
}
func EAM10components4mode11operational7program(state chsm.State, event chsm.Event) {
//#line 107 "microwave.chsmg"

            if event == Digit {
              
              
              
              
              
              
              machine.minutes = 0
              machine.seconds = Digit.DigitParam().n
              machine.PowerLevel = 100
            }
            machine.DisplayTimer()
          
}
func XAM10components4mode11operational7program(state chsm.State, event chsm.Event) {
//#line 121 "microwave.chsmg"

            if event != Open {
              components.mode.operational.program.Clear()
            }
          
}
func A2(event chsm.Event) {
	machine := event.Machine().(*Microwave)
	_ = machine
//#line 131 "microwave.chsmg"

              ShiftAdd(&minutes, seconds / 10)
              ShiftAdd(&seconds, Digit.DigitParam().n)
              machine.DisplayTimer()
            
}
func C1(event chsm.Event) bool {
	machine := event.Machine().(*Microwave)
	_ = machine
//#line 143 "microwave.chsmg"
		return  Digit.DigitParam().n > 0 
}
func A3(event chsm.Event) {
	machine := event.Machine().(*Microwave)
	_ = machine
//#line 143 "microwave.chsmg"

              machine.PowerLevel = Digit.DigitParam().n * 10
              fmt.Printf("Power: %d%%\n", PowerLevel)
            
}
func A4(event chsm.Event) {
	machine := event.Machine().(*Microwave)
	_ = machine
//#line 156 "microwave.chsmg"

            machine.minutes++;
            machine.DisplayTimer()
          
}
func EAM10components4mode11operational8set_time(state chsm.State, event chsm.Event) {
//#line 166 "microwave.chsmg"

            if event != Close {
              machine.old_hour = machine.hour
              machine.old_minute = machine.minute
              machine.hour = 0
              machine.minute = 0
            }
            DisplayTime()
          
}
func XAM10components4mode11operational8set_time(state chsm.State, event chsm.Event) {
//#line 175 "microwave.chsmg"

            if event != Open {
              if ( !OKTime() ) {
                machine.hour = machine.old_hour
                machine.minute = machine.old_minute
              }
              machine.DisplayTime()
            }
          
}
func A5(event chsm.Event) {
	machine := event.Machine().(*Microwave)
	_ = machine
//#line 188 "microwave.chsmg"

              ShiftAdd(&machine.hour, machine.minute/10)
              ShiftAdd(&machine.minute, Digit.DigitParam().n)
              machine.DisplayTime()
            
}
func C2(event chsm.Event) bool {
	machine := event.Machine().(*Microwave)
	_ = machine
//#line 193 "microwave.chsmg"
		return  ok_time() 
}
func C3(event chsm.Event) bool {
	machine := event.Machine().(*Microwave)
	_ = machine
//#line 194 "microwave.chsmg"
		return !ok_time() 
}
func A6(event chsm.Event) {
	machine := event.Machine().(*Microwave)
	_ = machine
//#line 194 "microwave.chsmg"

              fmt.Println("Time: EE")
            
}
func C4(event chsm.Event) bool {
	machine := event.Machine().(*Microwave)
	_ = machine
//#line 223 "microwave.chsmg"
		return  Digit.DigitParam().n > 0 
}

//#line 235 "microwave.chsmg"

///// CHSM: THE END


func main() {
  oven := new(Microwave)
  oven.Debug(chsm.DebugAll)
  oven.Enter()

  reader := bufio.NewReader(os.Stdin)

  for {
    oven.DumpState()
    //
    // Simple user-interface just to get events into the machine.
    //
    fmt.Printf("\nEnter: 0-9, Cloc(K) (M)inute (P)ower (O)pen (C)lose (S)tart S(T)op (D)one: ")

    ch, _, err := reader.ReadRune()
    if err != nil {
      break
    }

    if unicode.IsDigit(ch) {
      oven.Digit(ch - '0')
    } else {
      switch unicode.ToUpper(ch) {
        case 'K': oven.Clock()
        case 'M': oven.Minute()
        case 'P': oven.Power()
        case 'O': oven.Open()
        case 'C': oven.Close()
        case 'S': oven.Start()
        case 'T': oven.Stop()
        case 'D': oven.Done()
        default : fmt.Println("Invalid input")
      } // switch
    }
  } // while
}



