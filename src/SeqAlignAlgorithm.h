/*
 *   SeqAlignAlgorithm.h
 *
 */

#ifndef FLEXBAR_SEQALIGNALGORITHM_H
#define FLEXBAR_SEQALIGNALGORITHM_H


template <typename TSeqStr>
class SeqAlignAlgorithm {

private:
	
	typedef typename seqan::Value<TSeqStr>::Type        TChar;
	typedef typename seqan::Row<flexbar::TAlign>::Type  TRow;
	typedef typename seqan::Iterator<TRow>::Type        TRowIterator;
	
	typedef AlignResults<TSeqStr> TAlignResults;
	
	typedef seqan::Score<int, seqan::Simple>                              TScoreSimple;
	typedef seqan::Score<int, seqan::ScoreMatrix<TChar, seqan::Default> > TScoreMatrix;
	
	TScoreSimple m_score;
	TScoreMatrix m_scoreMatrix;
	
	const bool m_randTag;
	const flexbar::LogAlign m_log;
	const flexbar::TrimEnd m_trimEnd;
	
public:
	
	SeqAlignAlgorithm(const Options &o, const int match, const int mismatch, const int gapCost, const flexbar::TrimEnd trimEnd):
			m_randTag(o.randTag),
			m_log(o.logAlign),
			m_trimEnd(trimEnd){
		
		using namespace seqan;
		
		m_score = Score<int, Simple>(match, mismatch, gapCost);
		
		m_scoreMatrix = TScoreMatrix(gapCost);
		
		for(unsigned i = 0; i < ValueSize<TChar>::VALUE; ++i){
			for(unsigned j = 0; j < ValueSize<TChar>::VALUE; ++j){
				
				if(i == j || TChar(j) == 'N')
					 setScore(m_scoreMatrix, TChar(i), TChar(j), match);
				else setScore(m_scoreMatrix, TChar(i), TChar(j), mismatch);
			}
		}
		
		// printScoreMatrix(m_scoreMatrix);
	};
	
	
	virtual ~SeqAlignAlgorithm(){
	};
	
	
	void alignGlobal(TAlignResults &a, flexbar::TAlignments &alignments, const flexbar::ComputeCycle cycle, unsigned int &aIdx){
		
		using namespace std;
		using namespace seqan;
		using namespace flexbar;
		
		
		// TAlign align2;
		// TAlign &align = align2;
		//
		// if(m_randTag){
		//
		// 	resize(rows(align), 2);
		// 	assignSource(row(align, 0), readSeq);
		// 	assignSource(row(align, 1), querySeq);
		//
		// 	if(m_trimEnd == RIGHT || m_trimEnd == RIGHT_TAIL){
		//
		// 		AlignConfig<true, false, true, true> ac;
		// 		alScore = globalAlignment(align, m_scoreMatrix, ac);
		// 	}
		// 	else if(m_trimEnd == LEFT || m_trimEnd == LEFT_TAIL){
		//
		// 		AlignConfig<true, true, false, true> ac;
		// 		alScore = globalAlignment(align, m_scoreMatrix, ac);
		// 	}
		// 	else{
		// 		AlignConfig<true, true, true, true> ac;
		// 		alScore = globalAlignment(align, m_scoreMatrix, ac);
		// 	}
		// }
		// else{
			if(cycle == COMPUTE){
				
				if(m_trimEnd == RIGHT || m_trimEnd == RIGHT_TAIL){
					
					AlignConfig<true, false, true, true> ac;
					alignments.second = globalAlignment(alignments.first, m_score, ac);
				}
				else if(m_trimEnd == LEFT || m_trimEnd == LEFT_TAIL){
					
					AlignConfig<true, true, false, true> ac;
					alignments.second = globalAlignment(alignments.first, m_score, ac);
				}
				else{
					AlignConfig<true, true, true, true> ac;
					alignments.second = globalAlignment(alignments.first, m_score, ac);
				}
			}
			
			TAlign &align = value(alignments.first,  aIdx);
			a.score       = value(alignments.second, aIdx);
		// }
		
		// cout << "Score: " << alScore << endl;
		// cout << "Align: " << align << endl;
		
		
		TRow &row1 = row(align, 0);
		TRow &row2 = row(align, 1);
		
		a.startPosS = toViewPosition(row1, 0);
		a.startPosA = toViewPosition(row2, 0);
		a.endPosS   = toViewPosition(row1, length(source(row1)));
		a.endPosA   = toViewPosition(row2, length(source(row2)));
		
		if(a.startPosA > a.startPosS) a.startPos = a.startPosA;
		else                          a.startPos = a.startPosS;
		
		if(a.endPosA > a.endPosS) a.endPos = a.endPosS;
		else                      a.endPos = a.endPosA;
		
		
		// cout << "\n\n" << startPosS << endl << startPosA << endl << endPosS << endl << endPosA;
		// cout << align << endl << alScore << endl;
		
		if(m_log != flexbar::NONE){
			stringstream al;
			al << align;
			a.alString = al.str();
		}
		
		TRowIterator it1 = begin(row1);
		TRowIterator it2 = begin(row2);
		
		if(m_randTag) a.randTag = "";
		
		int aliPos   = 0;
		a.gapsR      = 0;
		a.gapsA      = 0;
		a.mismatches = 0;
		
		for(; it1 != end(row1); ++it1){
			
			if(a.startPos <= aliPos && aliPos < a.endPos){
				     if(isGap(it1))                   ++a.gapsR;
				else if(isGap(it2))                   ++a.gapsA;
				else if(*it1 != *it2 && *it2 != 'N')  ++a.mismatches;
				else if(m_randTag    && *it2 == 'N')  append(a.randTag, (TChar) *it1);
			}
			++aliPos;
			++it2;
		}
		
		// cout << "\n\n" << gapsR << endl << gapsA << endl << mismatches << endl;
		
		++aIdx;
	}
	
	
	void printScoreMatrix(TScoreMatrix &scoreMatrix){
		
		using namespace std;
		using namespace seqan;
		
		cout << endl;
		for(unsigned i = 0; i < ValueSize<TChar>::VALUE; ++i)
			cout << "\t" << TChar(i);
		cout << endl;
		
		for(unsigned i = 0; i < ValueSize<TChar>::VALUE; ++i){
			cout << TChar(i);
			for(unsigned j = 0; j < ValueSize<TChar>::VALUE; ++j)
				cout << "\t" << score(scoreMatrix, TChar(i), TChar(j));
			cout << endl;
		}
	}
};


#endif
