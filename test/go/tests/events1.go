

/**
 * @file
 * Tests that transitions are inherited properly and that parameter blocks are
 * set properly for derived events.
 */

package main

import (
  "chsm"
)

const TestValue = 42

///// CHSM: <<CHSM 6.0>>

///// CHSM: user-declarations

//#line 60 "tests/events1.chsmg"
///// CHSM: CHSM struct declaration

type TestMachine struct {
	chsm.CoreMachine

	// events
	AlphaEvent chsm.CoreEvent
	AlphaEventPb AlphaEventParamBlock
	BetaEvent chsm.CoreEvent
	BetaEventPb BetaEventParamBlock

	// states
	root chsm.CoreCluster
	S struct {
		chsm.CoreSet
		P struct {
			chsm.CoreCluster
			A chsm.CoreState
			B chsm.CoreState
		}
		Q struct {
			chsm.CoreCluster
			A chsm.CoreState
			B chsm.CoreState
		}
	}
}

// New function
func NewTestMachine() *TestMachine {
	machine := new(TestMachine)

	// initialize events
	machine.AlphaEvent.InitEvent(
		machine, "Alpha",
		[]chsm.TransitionId{1},
		/*baseEvent=*/nil,
	)
	machine.BetaEvent.InitEvent(
		machine, "Beta",
		[]chsm.TransitionId{0},
		/*baseEvent=*/&machine.AlphaEvent,
	)

	// initialize states
	machine.root.InitCluster(
		&machine.CoreMachine, /*id=*/0, "root",
		/*parent=*/nil,
		/*enterAction=*/nil, /*exitAction=*/nil,
		/*enterEvent=*/nil, /*exitEvent=*/nil,
		[]chsm.State{
			&machine.S,
		},
		/*history=*/false,
	)
	machine.S.InitSet(
		&machine.CoreMachine, /*id=*/1, "S",
		/*parent=*/&machine.root,
		/*enterAction=*/nil, /*exitAction=*/nil,
		/*enterEvent=*/nil, /*exitEvent=*/nil,
		[]chsm.State{
			&machine.S.P,
			&machine.S.Q,
		},
	)
	machine.S.P.InitCluster(
		&machine.CoreMachine, /*id=*/2, "S.P",
		/*parent=*/&machine.S,
		/*enterAction=*/nil, /*exitAction=*/nil,
		/*enterEvent=*/nil, /*exitEvent=*/nil,
		[]chsm.State{
			&machine.S.P.A,
			&machine.S.P.B,
		},
		/*history=*/false,
	)
	machine.S.P.A.InitState(
		&machine.CoreMachine, /*id=*/3, "S.P.A",
		/*parent=*/&machine.S.P,
		/*enterAction=*/nil, /*exitAction=*/nil,
		/*enterEvent=*/nil, /*exitEvent=*/nil,
	)
	machine.S.P.B.InitState(
		&machine.CoreMachine, /*id=*/4, "S.P.B",
		/*parent=*/&machine.S.P,
		/*enterAction=*/nil, /*exitAction=*/nil,
		/*enterEvent=*/nil, /*exitEvent=*/nil,
	)
	machine.S.Q.InitCluster(
		&machine.CoreMachine, /*id=*/5, "S.Q",
		/*parent=*/&machine.S,
		/*enterAction=*/nil, /*exitAction=*/nil,
		/*enterEvent=*/nil, /*exitEvent=*/nil,
		[]chsm.State{
			&machine.S.Q.A,
			&machine.S.Q.B,
		},
		/*history=*/false,
	)
	machine.S.Q.A.InitState(
		&machine.CoreMachine, /*id=*/6, "S.Q.A",
		/*parent=*/&machine.S.Q,
		/*enterAction=*/nil, /*exitAction=*/nil,
		/*enterEvent=*/nil, /*exitEvent=*/nil,
	)
	machine.S.Q.B.InitState(
		&machine.CoreMachine, /*id=*/7, "S.Q.B",
		/*parent=*/&machine.S.Q,
		/*enterAction=*/nil, /*exitAction=*/nil,
		/*enterEvent=*/nil, /*exitEvent=*/nil,
	)

	// initialize machine
	machine.InitMachine(
		&machine.root,
		[]chsm.State{
			&machine.S,
			&machine.S.P,
			&machine.S.P.A,
			&machine.S.P.B,
			&machine.S.Q,
			&machine.S.Q.A,
			&machine.S.Q.B,
		},
		[]chsm.Transition{
			{C1, &machine.S.P.A, &machine.S.P.B, nil, nil},
			{nil, &machine.S.Q.A, &machine.S.Q.B, nil, A1},
		},
	)

	return machine
}

///// CHSM: event definitions

type AlphaEventParamBlock struct {
	chsm.CoreEventParamBlock
	int value
}
func (machine *TestMachine) Alpha(int value) {
	var param AlphaEventParamBlock
	param.InitEventParamBlock(&machine.AlphaEvent)
	param.int = int
	machine.AlphaEvent.Broadcast(&param)
}

type BetaEventParamBlock struct {
	AlphaEventParamBlock
}
func (machine *TestMachine) Beta(int value) {
	var param BetaEventParamBlock
	param.InitEventParamBlock(&machine.BetaEvent)
	machine.BetaEvent.Broadcast(&param)
}


// user-code
func C1(event chsm.Event) bool {
	machine := event.Machine().(*TestMachine)
	_ = machine
//#line 45 "tests/events1.chsmg"
		return  false 
}
func A1(event chsm.Event) {
	machine := event.Machine().(*TestMachine)
	_ = machine
//#line 51 "tests/events1.chsmg"

          if Alpha.AlphaParam().value != TestValue {
            ExitCode = 2
          }
        
}

//#line 60 "tests/events1.chsmg"

///// CHSM: THE END


func main() {
  m := NewTestMachine()
  m.Enter()
  m.Debug(chsm.DebugAll)

  m.Beta(TestValue)

  m.DumpState()
  Check(m.Active() &&
    m.S.Active() &&
    m.S.P.Active() &&  m.S.P.A.Active() && !m.S.P.B.Active() &&
    m.S.Q.Active() && !m.S.Q.A.Active() &&  m.S.Q.B.Active(),
  )
  PrintResult()
}



