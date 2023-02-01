








package main

///// CHSM: <<CHSM 6.0>>

import "chsm"

///// CHSM: user-declarations

//#line 55 "infinite.chsmg"
///// CHSM: CHSM struct declaration

type Infinite struct {
	chsm.Machine

	// events
	XEvent chsm.Event
	YEvent chsm.Event

	// states
	root chsm.Cluster
	S struct {
		chsm.Set
		L struct {
			chsm.Cluster
			A chsm.State
			B chsm.State
		}
		R struct {
			chsm.Cluster
			C chsm.State
			D chsm.State
		}
	}
}

// New function
func NewInfinite() *Infinite {
	machine := new(Infinite)

	// initialize events
	machine.XEvent.InitEvent(
		machine, "X",
		[]chsm.TransitionID{0, 1},
		nil,
	)
	machine.YEvent.InitEvent(
		machine, "Y",
		[]chsm.TransitionID{2, 3},
		nil,
	)

	// initialize states
	machine.root.InitCluster(
		&machine.Machine, "root", nil, nil, nil, nil, nil,
		[]chsm.IState{
			&machine.S,
		},
		false,
	)
	machine.S.InitSet(
		&machine.Machine, "S", &machine.root, nil, nil, nil, nil,
		[]chsm.IState{
			&machine.S.L,
			&machine.S.R,
		},
	)
	machine.S.L.InitCluster(
		&machine.Machine, "S.L", &machine.S, nil, nil, nil, nil,
		[]chsm.IState{
			&machine.S.L.A,
			&machine.S.L.B,
		},
		false,
	)
	machine.S.L.A.InitState(
		&machine.Machine, "S.L.A", &machine.S.L, nil, nil, nil, nil,
	)
	machine.S.L.B.InitState(
		&machine.Machine, "S.L.B", &machine.S.L, nil, nil, nil, nil,
	)
	machine.S.R.InitCluster(
		&machine.Machine, "S.R", &machine.S, nil, nil, nil, nil,
		[]chsm.IState{
			&machine.S.R.C,
			&machine.S.R.D,
		},
		false,
	)
	machine.S.R.C.InitState(
		&machine.Machine, "S.R.C", &machine.S.R, nil, nil, nil, nil,
	)
	machine.S.R.D.InitState(
		&machine.Machine, "S.R.D", &machine.S.R, nil, nil, nil, nil,
	)

	// initialize machine
	machine.InitMachine(
		&machine.root,
		[]chsm.IState{
			&machine.S,
			&machine.S.L,
			&machine.S.L.A,
			&machine.S.L.B,
			&machine.S.R,
			&machine.S.R.C,
			&machine.S.R.D,
		},
		[]chsm.Transition{
			{nil, &machine.S.L.A, &machine.S.L.B, nil, A1},
			{nil, &machine.S.L.B, &machine.S.L.A, nil, A2},
			{nil, &machine.S.R.C, &machine.S.R.D, nil, A3},
			{nil, &machine.S.R.D, &machine.S.R.C, nil, A4},
		},
	)

	return machine
}

///// CHSM: event definitions

type XParamBlock struct {
	chsm.EventParamBlock
}
func (machine *Infinite) X() {
	machine.XEvent.Broadcast(nil)
}

type YParamBlock struct {
	chsm.EventParamBlock
}
func (machine *Infinite) Y() {
	machine.YEvent.Broadcast(nil)
}


// user-code
func A1(event chsm.IEvent) {
	machine := event.Machine().(*Infinite)
	_ = machine
//#line 40 "infinite.chsmg"
 machine.Y() 
}
func A2(event chsm.IEvent) {
	machine := event.Machine().(*Infinite)
	_ = machine
//#line 43 "infinite.chsmg"
 machine.Y() 
}
func A3(event chsm.IEvent) {
	machine := event.Machine().(*Infinite)
	_ = machine
//#line 48 "infinite.chsmg"
 machine.X() 
}
func A4(event chsm.IEvent) {
	machine := event.Machine().(*Infinite)
	_ = machine
//#line 51 "infinite.chsmg"
 machine.X() 
}

//#line 55 "infinite.chsmg"

///// CHSM: THE END


func main() {
  m := NewInfinite()
  m.Enter()

  m.X()
}



