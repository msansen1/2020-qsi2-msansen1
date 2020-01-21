open Framework;
open QCheckRely;
open Generator.Fantasy;
open Lib.Troll;
open Lib.Elf;

let {describe} = extendDescribe(QCheckRely.Matchers.matchers);

describe("Troll Invariance", ({test}) => {
  test("Troll score should be 0 when all elves resurrected", ({expect}) => {
    QCheck.Test.make(
      ~count=1000,
      ~name="Troll score should be 0 when all elves resurrected",
      troll_arbitrary,
      troll =>
      all_elves_resurrected(troll) |> scoring == 0
    )
    |> expect.ext.qCheckTest;
    ();
  });

  test("Troll score should always be >= 0", ({expect}) => {
    QCheck.Test.make(
      ~count=1000,
      ~name="Troll score should always be >= 0",
      troll_arbitrary,
      troll =>
      all_elves_resurrected(troll) |> scoring >= 0
    )
    |> expect.ext.qCheckTest;
    ()
  });
});

describe("Troll Inverse", ({test}) => {
  test("oops_he_survived should always be inverse of i_got_one", ({expect}) => {
    QCheck.Test.make(
      ~count=1000,
      ~name="oops_he_survived should always be inverse of i_got_one",
      troll_elf_arbitrary,
      ((troll,elf)) => i_got_one(elf,troll) |> oops_he_survived(elf) |> scoring == scoring(troll)
    )
    |> expect.ext.qCheckTest;
    ()
  });
});

describe("Troll Analogy", ({test}) => {
  test("i_got_one and i_got should be consistent", ({expect}) => {
    //fold left plutot qu'un for pour bouler 
    QCheck.Test.make(
      ~count=1000,
      ~name="i_got_one and i_got should be consistent",
      troll_elf_int_arbitrary,
      ((troll,elf,nbre)) => {
        let trollMultiGotOne = ref(troll);//ref pour que le contenu soit mutable
        for(i in 1 to nbre){
          //On assigne la nouvelle value a trollMultiGotOne
          trollMultiGotOne := i_got_one(elf,trollMultiGotOne^); //^pour recuperer ce qui est pointé par le pointeur ref
          };
        (trollMultiGotOne^ |> scoring ) == (troll |> i_got(nbre, elf) |> scoring)
        //(trollMultiGotOne^ |> scoring ) == ( i_got(nbre, elf,troll) |> scoring)
        //|> redirige le parametre de gauche a droite de la partie droite
        //f(g(h(x))) === h(x) |> g |> f === x |> h |> g |> f
      }
    )
    |> expect.ext.qCheckTest;
    ()
  });
});

describe("Troll Idempotence", ({test}) => {
  test(
    "all_elves_of_a_kind_resurrected brings the Troll killing list to a stable state",
    ({expect}) => {
      QCheck.Test.make(
        ~count=1000,
        ~name="all_elves_of_a_kind_resurrected brings the Troll killing list to a stable state",
        troll_elf_arbitrary,
        ((troll,elf)) => {
          let test = all_elves_of_a_kind_resurrected(elf,troll);
          let test2 = all_elves_of_a_kind_resurrected(elf,troll);
          (test |> scoring ) == ( test2 |> scoring )
        }
      )
      |> expect.ext.qCheckTest;
      ()
  })
  test(
    "all_elves_resurrected brings the Troll killing list to a stable state",
    ({expect}) => {
      QCheck.Test.make(
        ~count=1000,
        ~name="all_elves_resurrected brings the Troll killing list to a stable state",
        troll_elf_arbitrary,
        ((troll,elf)) => {
          let test = all_elves_resurrected(troll);
          let test2 = all_elves_resurrected(troll);
          (test |> scoring ) == ( test2 |> scoring )
        }
      )
      |> expect.ext.qCheckTest;
      ()
  })
});

describe("Troll Metamorphism", ({test}) => {
  test(
    "ensure score inscrese after a kill",
    ({expect}) => {
      QCheck.Test.make(
      ~count=1000,
      ~name="ensure score inscrese after a kill",
      troll_elf_arbitrary,
      ((troll, elf)) => {
        //Score after a kill must be more than before the kill
        let scoreBefore = scoring(troll);
        let scoreAfterOneKill = scoring( i_got_one(elf, troll) );
        ( scoreAfterOneKill >  scoreBefore )
      }
    )
    |> expect.ext.qCheckTest;
    ()
  })
});

describe("Troll Injection", ({test}) => {
  test(
    "comparing elf value",
    ({expect}) => {
      QCheck.Test.make(
      ~count=1000,
      ~name="comparing elf value",
      troll_two_elves_arbitrary,
      ((troll, elf1, elf2)) => {
        //etat du troll apres kill elf1
        let killElf1 = i_got_one(elf1, troll);
        //etat du troll apres kill elf2
        let killElf2 = i_got_one(elf2, troll);
        //le test plante car certains elfs ont la meme value
        if (value(elf1) == value(elf2)) {
          //{ Elf.role = Elf.Archer; race = Elf.DarkElf } = 2, { Elf.role = Elf.Swordsman; race = Elf.HighElf } = 2)
          ( killElf1 |> scoring ) == ( killElf2 |> scoring );
        } else {
          //{ Elf.role = Elf.Warlock; race = Elf.HighElf } = 8, { Elf.role = Elf.Swordsman; race = Elf.HighElf } = 2)
          ( killElf1 |> scoring ) != ( killElf2 |> scoring );
        };
      }
    )
    |> expect.ext.qCheckTest;
    ()
  })
});