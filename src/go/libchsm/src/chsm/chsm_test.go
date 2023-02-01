

/**
 * @file
 * Tests that enter/exit code works.
 */

package chsm

import (
	"testing"
)

///// CHSM: <<CHSM 6.0>>

///// CHSM: user-declarations

//#line 61 "tests/enter_exit.chsmg"
///// CHSM: CHSM struct declaration

type TestMachine struct {
	CoreMachine

	// events
	AlphaEvent CoreEvent
	AlphaEventPb AlphaEventParamBlock
	BetaEvent CoreEvent
	BetaEventPb BetaEventParamBlock

	// states
	root CoreCluster
	A CoreState
	B CoreState
}

// New function
func NewTestMachine() *TestMachine {
	machine := new(TestMachine)

	// initialize events
	machine.AlphaEvent.InitEvent(
		machine, "Alpha",
		[]TransitionId{0},
		/*baseEvent=*/nil,
	)
	machine.BetaEvent.InitEvent(
		machine, "Beta",
		[]TransitionId{},
		/*baseEvent=*/&machine.AlphaEvent,
	)

	// initialize states
	machine.root.InitCluster(
		&machine.CoreMachine, /*id=*/0, "root",
		/*parent=*/nil,
		/*enterAction=*/nil, /*exitAction=*/nil,
		/*enterEvent=*/nil, /*exitEvent=*/nil,
		[]State{
			&machine.A,
			&machine.B,
		},
		/*history=*/false,
	)
	machine.A.InitState(
		&machine.CoreMachine, /*id=*/1, "A",
		/*parent=*/&machine.root,
		/*enterAction=*/nil, /*exitAction=*/nil,
		/*enterEvent=*/nil, /*exitEvent=*/nil,
	)
	machine.B.InitState(
		&machine.CoreMachine, /*id=*/2, "B",
		/*parent=*/&machine.root,
		/*enterAction=*/nil, /*exitAction=*/nil,
		/*enterEvent=*/nil, /*exitEvent=*/nil,
	)

	// initialize machine
	machine.InitMachine(
		&machine.root,
		[]State{
			&machine.A,
			&machine.B,
		},
		[]Transition{
			{nil, &machine.A, &machine.B, nil, nil},
		},
	)

	return machine
}
///// CHSM: event definitions

type AlphaEventParamBlock struct {
	CoreEventParamBlock
}
func (machine *TestMachine) Alpha() {
	machine.AlphaEvent.Broadcast(nil)
}

type BetaEventParamBlock struct {
	AlphaEventParamBlock
}
func (machine *TestMachine) Beta() {
	machine.BetaEvent.Broadcast(nil)
}

var ExitCode = 1

func TestCHSM(t *testing.T) {
	m := NewTestMachine()
	m.Enter()
	m.Debug(DebugAll)

	m.Beta()

	m.DumpState()
}