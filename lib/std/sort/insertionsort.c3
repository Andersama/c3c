module std::sort;
import std::sort::is;

macro insertionsort(list, cmp = null) @builtin
{
	var $Type = $typeof(list);
	var $CmpType = $typeof(cmp);
	usz len = sort::@len_from_list(list);
	is::isort(<$Type, $CmpType>)(list, 0, (usz)len, cmp);
}

module std::sort::is(<Type, Comparer>);

def ElementType = $typeof(Type{}[0]);

fn void isort(Type list, usz low, usz high, Comparer comp) {
	for (usz i = low; i < high; ++i) {
		usz j = i;
		for (;j > low;) {
      ElementType *rhs = list.get_ref(j);
      ElementType *lhs = list.get_ref(--j);
			if (!comp(rhs, lhs)) {
				break;
      }
			@swap(*rhs, *lhs);
		}
	}
}
